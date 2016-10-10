/* mehPL:
 *    This is Open Source-ish, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//heartbeat 1.20
#include "heartbeat.h"


//So far this is only used with HEART_DMS to disallow cumulative-error in
//heart-updates... 
// This should also be used with HEART_TCNTER, soon (if not already)

//It shouldn't be necessary to be TRUE in the vast-majority of cases
// Setting this true bumped the test program from 1550 to 1572 bytes...
#define HEART_PRECISE   FALSE

#if(_HEART_TCNTER_)

#include _TCNTER_HEADER_

// Now defined in tcnter.h
// #ifndef TCNTER_MS
//  #warning "TCNTER_MS is completely arbitrary"
//  #define TCNTER_MS 100
// #endif

//HEART_TCNTER_UPDATES_AND_INIT can be CFLAG +='d...
// But if it's not, we're going to default to TRUE
// If your project uses tcnter_update() in the main-while loop, then set
// this FALSE (don't forget tcnter_init() in main)
// e.g. if tcnter is used by multiple things, like polled_uar/t
// (It could be left for heart_update() to handle, still... but as it
// stands, HEART_REMOVED would destroy that.)
 #ifndef HEART_TCNTER_UPDATES_AND_INIT
  #warning "HEART_TCNTER_UPDATES_AND_INIT is defaulting to TRUE, tcnter_update() will be run with each call of heart_update()"
  #define HEART_TCNTER_UPDATES_AND_INIT TRUE
 #endif

//100% = 255, 0% = 0...
//volatile uint8_t  newDesired = 0;

//Could be uint8 for dms and uint16 for nondms
tcnter_t heartStepTime = HEARTSTEPTIME;
#else
uint16_t heartStepTime = HEARTSTEPTIME;
#endif

//volatile uint8_t  heartSet = HEARTDONTSET;

#if (!defined(HEARTPIN_HARDCODED) || !HEARTPIN_HARDCODED)
//#define LED_DIRECT_HIGH
//#define LED_INVERTED
uint8_t ledConnection;

volatile uint8_t*	heartPIN;
uint8_t heartBeatPin;
#endif

//shift the update-period by heartRate to increase the rate
// this is a right-shift for calculation efficiency
// so rates are 0, 1, 2, 3 -> PERIOD, PERIOD/2, PERIOD/4, PERIOD/8...
//uint8_t heartRate = 0;

hfm_t heartModulator;

/*
void heartMode(uint8_t mode)
{
	switch(mode)
	{
		case HEART_2X:
			heartRate = 1;
			break;
		case HEART_4X:
			heartRate 
	
	}
	heartSet = mode;
}
*/

#if((defined(_HEART_DMS_) && _HEART_DMS_))
#warning "Relevant Everywhere: dms6sec_t used in heartbeat, yet a full cycle is 8 seconds, and nevermind blinks! This might need changing!"
#endif


#if((defined(_HEART_DMS_) && _HEART_DMS_) || \
	 (defined(_HEART_TCNTER_) && _HEART_TCNTER_))
	#define HEART_USES_TIMER	TRUE
#else
	#define HEART_USES_TIMER	FALSE
//	#define HEARTSTEPTIME	(HEART_ITERATIONCOUNT)
#endif


void heart_setRate(uint8_t rate)
{
//	heartRate = rate;
	
	if(rate == 0)
		rate = 1;
		
    heartStepTime = (HEARTSTEPTIME/rate);
}

#if (!defined(HEART_GETRATE_UNUSED) || !HEART_GETRATE_UNUSED)
//This function takes about 20 bytes
uint8_t heart_getRate(void)
{
	 return (uint8_t)(HEARTSTEPTIME/heartStepTime);
}
#endif

#if(defined(_HEART_DMS_) && _HEART_DMS_)
 #if (!defined(DMS_WAITFN_UNUSED) || !DMS_WAITFN_UNUSED)
// This only saves 4 bytes...
void heart_voidUpdate(void)
{
	heart_update();
}
 #endif
#endif

void heart_clear(void)
{
	hfm_setPower(&heartModulator, 0);
}


#if (!defined(HEARTPIN_HARDCODED) || !HEARTPIN_HARDCODED)
//THIS USES THE WDT NOW!
// AFTER heart_init, the wdt must be reset often enough!!! this can be accomplished via heart_update...
void heart_init(volatile uint8_t* pin, uint8_t pinName, uint8_t ledConnectionType)
{
//	heartDDR = ddr;
	heartPIN = pin;
	heartBeatPin = pinName;
	ledConnection = ledConnectionType;

//	setoutVar(heartBeatPin, heartPIN);
//	setout(PD6, DDRD);

	//Set the port first, so there's not a short 
	//(there may be an instant of floating on the input of an inverter)
	//PORT = L
	clrpinVar(heartBeatPin, heartPIN);
	//DDR = H,out
	setoutVar(heartBeatPin, heartPIN);
#else
void heart_init(void)
{
 #if (!defined(HEART_NOPIN) || !HEART_NOPIN)
	clrpinPORT(HEART_PINNUM, HEART_PINPORT);
	setoutPORT(HEART_PINNUM, HEART_PINPORT);
 #endif
#endif



//	hfm_setPower(&heartModulator, 0);
	hfm_setup(&heartModulator, 0, HEART_MAXBRIGHT);

	heart_setRate(0);
	
#if (_HEART_DMS_)
 #if (defined(_DMS_EXTERNALUPDATE_) && _DMS_EXTERNALUPDATE_)
	//dmsTimer should be initialized elsewhere based on the rate of dmsUpdate calls...
 #else 
	init_dmsTimer();
 #endif

#elif(defined(HEART_TCNTER_UPDATES_AND_INIT) && \
		HEART_TCNTER_UPDATES_AND_INIT)
	tcnter_init();
#endif


#if (!defined(_WDT_DISABLE_) || !_WDT_DISABLE_)
	// IF the reset occurred, the WDTimer is reset to 16ms!
	// IF wdt_reset() isn't called before then, it will get stuck resetting!
	//!!!! This can happen even if the WDT is NEVER ENABLED
	// esp. if the bit is set during a brown-out...
	wdt_reset();
	// So also disable it (and reenable it if we want it)
	wdt_disable();

	//Check if there was a watchdog reset...
	// IF the reset occurred, the WDTimer is reset to 16ms!
	// IF wdt_reset() isn't called before then, it will get stuck resetting!
	//!!!! This can happen even if the WDT is NEVER ENABLED
	// esp. if the bit is set during a brown-out...
	//Could also check ! Power-On, and/or brownout... 
	if(getbit(WDRF, MCUStatReg))
	{
		heart_setRate(32);
/*
		
		//HALT HERE FOR DEBUGGING!!!
		// INDICATE it on the HeartBeat!
#warning "watchdog should NOT hang the system when distributed... this is DEBUGGING ONLY"
		while(1)
		{
			heart_update();
		}
*/
	}
	if(!getbit(PORF, MCUStatReg))
	{
		//It seems unusual that a brown-out wouldn't occur on power-up... takes time to charge caps, etc.
		//only check this if PowerOnReset was already cleared from a previous run
		if(getbit(BORF, MCUStatReg))
		{
			heart_setRate(16);
		}
	}
	//Clear the reset flags so we can see next time...
//	MCUSR = 0x0f;
	MCUStatReg = 0;
	
	//It's kinda handy to have the WDT run for a while so we can tell it crashed...
 #if defined(_WDTO_USER_)
	wdt_enable(_WDTO_USER_);
 #else
  #if defined(WDTO_4S)
	wdt_enable(WDTO_4S);
  #else
	wdt_enable(WDTO_1S);
  #endif
 #endif
#else
 #warning "WDT DISABLED"
#endif
}

//Increment or decrement the heart brightness as appropriate
// this is called periodically by heart_update...
// This function takes about 32 bytes
// and inlining it saved 6, wee!
static __inline__ uint8_t getNextDesired(void)
{
	//Don't set abs(direction) > 1 or we'll have wraparound errors!
	static int8_t direction = +1;
	int16_t desired;


//Somehow it seems the program gets stuck with the heartbeat at half-brightness...
//So main is still running, but the heart isn't incrementing!
// So do the watchdog reset here...

//!!! Check This with WDT...
//#warning "at what heart_reset() rate will getNextDesired never be called?"
#if (!defined(_WDT_DISABLE_) || !_WDT_DISABLE_)
	//Woot! Saved two bytes!
	wdt_reset();
#endif

	desired = (uint8_t)(heartModulator.power);

	desired += direction;
	
	//Slightly slower not to have these internal to above, but also safer...
	
	if(desired >= HEART_MAXBRIGHT)
	{
		desired = HEART_MAXBRIGHT;
		direction = -1;
	}	

   //Why is this not else-if...?
	if(desired <= 0)
	{
		desired = 0;
		direction = +1;
	}
	
	return (uint8_t)desired;
}

#if (!defined(HEART_BLINK_UNUSED) || !HEART_BLINK_UNUSED)
//Used for error-indication...
// when 0, the heart fades as usual
// when non-zero the heart blinks as follows:
//  say heartBlink is 0xXY, it will blink Y times first, then X times
//  then repeat...
// (actually, I think that's backwards, a remnant of times past... I think
// it's now X times first, then Y...)
uint8_t heartBlink = 0;
//#define TOGGLETIME	(250*DMS_MS)

void heart_blink(uint8_t count)
{
	heartBlink = count;
}





//Called by heart_update... used internally, only
static __inline__
uint8_t blinkHeart(void)
		__attribute__((__always_inline__));


#if (_HEART_DMS_)
 #define heartTime_t dms6sec_t
 #define heart_isItTime(a,b,c)	dmsIsItTime6secV2((a),(b),(c))
 #define TOGGLETIME   (250*DMS_MS)
//This function takes about 288 bytes
 #warning "This may get cut, if the blink-rate is longer than 6sec!"


#elif (defined(_HEART_TCNTER_) && _HEART_TCNTER_)
 #define heartTime_t myTcnter_t
 #define heart_isItTime(a,b,c)	tcnter_isItTimeV2((a),(b),(c))
 #define TOGGLETIME (250*TCNTER_MS)


#else
 #define heartTime_t uint32_t

 heartTime_t currentTime; 

 static __inline__
 uint8_t heart_isItTime(heartTime_t *startTime, heartTime_t deltaTime,
                                                            uint8_t nyi)
 {
      heartTime_t thisDelta = currentTime - *startTime;
      if(thisDelta >= deltaTime)
      {
         *startTime = currentTime;
         return TRUE;
      }
      else
         return FALSE;
 }

 #warning "Prior to v2.00, blinkHeart() was NOT static inline for ONLY this case where neither HEART_DMS nor HEART_TCNTER are true. I can't think of why that would be. So It's now inline for all cases... But it's untested."
 #define TOGGLETIME   (2500)
#endif


uint8_t blinkHeart(void)
{
	static heartTime_t lastToggleTime = 0;

	static uint8_t toggleCount = 0;

	//This is not THE high nibble
	// This is True or False depending on 
	//  whether thisNibble is the high nibble
	static uint8_t highNibble = 0;
	

	uint8_t thisNibble = heartBlink;

	//If both nibbles are filled, process them as specified
	if((heartBlink & 0xf0) && (heartBlink & 0x0f))
	{	//Fall through and toggle...
	}
	//Otherwise just process the one that's filled
	// to avoid long delays and hopefully make it easier to determine which
	// nibble is filled...
	else if(heartBlink & 0x0f)
		highNibble = 0;
	else if(heartBlink & 0xf0)
		highNibble = 1;

	//Fill the appropriate nibble for blinking...
	if(highNibble)
		thisNibble = (thisNibble & 0xf0) >> 4;
	else
		thisNibble = (thisNibble & 0x0f);

	//Blink the proper number of times
	if((toggleCount>>1) < thisNibble)
	{
		if(heart_isItTime(&lastToggleTime, TOGGLETIME, HEART_PRECISE))
			toggleCount++;
	}
	//Wait for a while
//Has been working for quite some time, so removing this warning...
//#warning "There may be some integer promotion issues here..."
	else if(heart_isItTime(&lastToggleTime, 
						((((uint32_t)TOGGLETIME) * 8)<<(highNibble)),
						HEART_PRECISE))
	{
		toggleCount = 0;
		//Only valid if both nibbles are filled (see above)
		highNibble = !highNibble;
	}

	//(actually, the new value, at this point... but will be the last soon)
	return !getbit(0, toggleCount);
}
#endif








//Not that these names aren't particularly useful, but this is how they
//were...
//Note, also, that On/Off was reversed (?!)
#ifdef HEART_LEDCONNECTION
 #if(HEART_LEDCONNECTION == LED_DIRECT_HIGH)
   #define LED_ON_PINSTATE    PINSTATE_PU
 #elif(HEART_LEDCONNECTION == LED_INVERTED)
   #define LED_ON_PINSTATE    PINSTATE_HIGH
 #else
   #error "Not Handled"
 #endif
 #define LED_OFF_PINSTATE   PINSTATE_LOW
 #define BUTTON_ON_PINVAL   PINSTATE_LOW
 #define BUTTON_INPUT_PINSTATE   PINSTATE_PU
#endif



#if (!defined(HEART_INPUTPOLLING_UNUSED) || !HEART_INPUTPOLLING_UNUSED)
// This value's arbitrary... works with tabletBluetoother 3...
// This function takes about 20 bytes
// HEART_PULLUP_DELAY was originally 3, but doesn't seem to be enough for
// the pwm161 running with CLKDIV8 disabled.
#ifndef HEART_PULLUP_DELAY
 #define HEART_PULLUP_DELAY 255
 #warning "HEART_PULLUP_DELAY is unset, defaulting to a large value"
 #warning " You can add CFLAGS += -D'HEART_PULLUP_DELAY=<value>' to your makefile"
#endif

volatile uint8_t heartButtonVal;
#endif

#if (!defined(HEART_NOPIN) || !HEART_NOPIN)
//These are INTERNAL-ONLY... there's another function for getting the
//button-value externally, which handles switching pin direction, waiting
//for pull-ups to overcome capacitance, etc.
static __inline__
uint8_t heartI_getButtonVal(void)
{
   //This is valid because PINSTATE_L = 0 and PINSTATE_H = 1
	return (getpinPORT(HEART_PINNUM, HEART_PINPORT) == BUTTON_ON_PINVAL);
}

static __inline__
void heartI_switchToInput(void)
{
   
   //Set the pin as an input, pulled-up if necessary.
   setPinStatePORT(HEART_PINNUM, HEART_PINPORT, BUTTON_INPUT_PINSTATE);
}

static __inline__
uint8_t heartI_setLED(uint8_t ledOn)
{

	if(ledOn)
	{
      setPinStatePORT(HEART_PINNUM, HEART_PINPORT, LED_ON_PINSTATE);
		return TRUE;
	}
	else
	{
      setPinStatePORT(HEART_PINNUM, HEART_PINPORT, LED_OFF_PINSTATE);
		return FALSE;
	}
}
#else 
//HEART_NOPIN is TRUE, these need to be defined in heartStuff.h
// in your project directory...
#include "heartNoPinStuff.h"
extern __inline__
uint8_t heartI_getButtonVal(void);

extern __inline__
void heartI_switchToInput(void);

//This needs to return ledOn...
extern __inline__
uint8_t heartI_setLED(uint8_t ledOn);

#endif //HEART_NOPIN


//This function takes about 186 bytes
// Really?! Or is hfm not compiled if it's not used?
//a/o v2.00:
//  Notes, in here anyhow, stated that this returns TRUE if the pin was
//  avaialble-for-input... This was implementation-specific, and not really
//  implemented.
//  Now, I think, it's not relevent.
//  Regardless, the actual return-value was the state of the LED
//  Which is the case, now, except when sampling the pin...
uint8_t heart_update(void)
{	
	static heartTime_t lastTime = 0;

#if (	HEART_USES_TIMER )
 #if ( _HEART_TCNTER_ )
  #if (HEART_TCNTER_UPDATES_AND_INIT)
	tcnter_update();
  #endif
 #endif
	//oldNote:
   //HEARTSTEPTIME isn't 100% accurate
   // especially if this isn't accessed often enough...
#else
	currentTime++;
   //oldNotes:
//	if(currentTime - lastTime > (HEART_ITERATIONCOUNT>>heartRate))
	//heartStepTime is number of updates, no corelation with actual seconds.
#endif



#if (!defined(HEART_INPUTPOLLING_UNUSED) || !HEART_INPUTPOLLING_UNUSED)
   static uint8_t heartSamplingPin;
#define heartPinSampleTime HEART_PULLUP_DELAY

   //Sample the pin between each heartStep
   //That should be plenty fast-enough for any reasonable button-press
   //While it also shouldn't be so often that it interferes with the fading
   //TODO: What when heart_setRate is used...? 
   if(heartSamplingPin)
   {
      //Wait for the pull-up to overcome the capacitance (if necessary)
      if(heart_isItTime(&lastTime, heartPinSampleTime, HEART_PRECISE))
      {
         heartButtonVal =  heartI_getButtonVal();

         heartSamplingPin = FALSE;
         
         hfm_setPower(&heartModulator, getNextDesired());
      }

      //THIS ISN'T REALLY USEFUL...
      // There *could maybe* be a test to see whether the heartLED *would
      // be on* and whether the LED would also be on given the button-pin's
      // input state... but that'd be hokey
      // I suppose it could also return the input-value
      // (regardless of the pull-up's charging-state)
      // I dunno.
      return FALSE;
   }
   else
   {
	   if(heart_isItTime(&lastTime, (heartStepTime-heartPinSampleTime), 
                                                            HEART_PRECISE))
      {
         //Set the pin as an input, pulled-up if necessary.
         heartI_switchToInput();
         
         heartSamplingPin = TRUE;

         return FALSE;
      }
   }
#else //HEART_INPUTPOLLING_UNUSED
	if(heart_isItTime(&lastTime, heartStepTime, HEART_PRECISE))
         hfm_setPower(&heartModulator, getNextDesired());
#endif

   //We only get here if heartSamplingPin is FALSE

	//determine whether the LED should be on or off based on the HFM
   uint8_t ledVal;

#if (!defined(HEART_BLINK_UNUSED) || !HEART_BLINK_UNUSED)
   //Kinda funny, the heart's getNextDesired() is still run, even though
   //it's not used for heartBlink...
   if(heartBlink)
      ledVal = blinkHeart();
   else
#endif
      ledVal = hfm_nextOutput(&heartModulator);

   return heartI_setLED(ledVal);
}















//                           .
//                 H--|>---. .
//                     o   | .
//                     |   / .
// ledOn && !input >---+   \ .
//                     |   / .
//                     |   | .
//                 L--|>---+-------+---|<|---/\/\/\----> V+
//                         | .     |    
//                 <--<|---' .     |   _|_
//                           .     '---o o---> GND




//                  .
//        H--|>---. .
//                | .
//                / .
//                \ .
//                / .         Not close enough to Low or High for Pull-Up
//                | .       + 1.6 -
//        H--|>---+-------+---|>|---/\/\/\----> GND
//                | .     |
//        <--<|---' .         _|_
//                  .      ---o o---> ???
//  on  = HIGH
//  off = low, or input (not pulled-up)
//  
//  (input value would be an unknown state, would require resistors,
//   pull-up is not strong-enough to determine state)

// OPTIONS:                           On                Off
//   LED: Tied directly to high       Low               High, Z, Pulled-up
//        Tied directly to low        High              Low, Z, Pulled-Down
//        Tied via inverter to high   High, pulled-up   Low, Pulled-Down
//        Tied via inverter to low    Low, pulled-down  High, Pulled-Up
//        Tied via buffer to high     Low, pulled-down  High, pulled-up
//        Tied via buffer to low      High, pulled-up   Low, Pulled-Down
//                                            ^--MutuallyExclusive--*

//   Button/Switch:
//        Tied directly to low        Low               Pulled-Up
//        Tied directly to high       High              Pulled-Down
//        Tied via resistor to low    Low (small R)     Pulled-Up
//        Tied via resistor to high   High (small R)    Pulled-Down

//   Pull-Up/Pull-Down:
//        None
//        Internal Pull-Up
//        External Pull-Up
//        External Pull-Down

// OK, so there are too many options
// How about, intead...
// Have, e.g.
// LED_ON_PINSTATE = (H, L, Z, PU)
// LED_OFF_PINSTATE = (H, L, Z, PU)
// BUTTON_ON_PINVAL = (H, L)  <--- Always Opposite
// BUTTON_OFF_PINVAL = (H, L) <--/
// BUTTON_INPUT_PINSTATE = (Z, PU)

// (Then, could, e.g. 
//   if(ledPinstate == BUTTON_READ_PINSTATE)
//     buttonVal = getpinPORT(...) ...?)

#if (!defined(HEARTPIN_HARDCODED) || !HEARTPIN_HARDCODED)
void setPinState_wrapper(uint8_t pin, volatile uint8_t * P_in, uint8_t
                                                                  state)
{
   setPinStatePort(heartBeatPin, *(P_in+PORTOFFSET), state);
}

#error "HEARTPIN_HARDCODED = FALSE; development for variable heartPins has ceased long ago, and have basically given-up a/o v2.00"
//Really, what's the point of making it a variable?
//Development has ceased largely due to the fact that now we'd *also* need
//variables for the various PINSTATEs, etc... (or would those be hardcoded,
//and yet the pin itself stay variable...? I have no idea why it was ever
//variable in the first place. Maybe I'm just losing my mind.
#endif




























//a/o v2.00-2, revisiting the
//VARIOUS CONNECTION METHODS:
// Sources: 1.00-4, 1.10
//
// PREFERRED: Heart Tied High (LED_TIED_HIGH)
//                  .
//        H--|>---. .
//            o   | .
//            |   / .
// ledOn  >---+   \ .
//            |   / .
//            |   | .
//        L--|>---+-------+---|<|---/\/\/\----> V+
//                | .     |    
//        <--<|---' .     |   _|_
//                  .     '---o o--->GND
//
//		Without inverter, LED tied high: (LED_DIRECT_HIGH/LED_TIED_HIGH)
//     uC pin is either Low, or Pulled-Up (never High)
//			LED-ON		(DDR = H-out,	PORT = L)
//			LED-OFF/IN	(DDR = L-in,	PORT = H-pull-up)
//				(If IN is pulled LOW via the button, the LED will be ON)
//    BUTTON can only be momentary



//                  .
//        H--|>---. .
//            o   | .
//            |   / .<-- >=20k (internal)
// ledOn  >---+   \ .
//            |   / .
//            |   | .
//        L--|>---+-------+---|>o-----|<|---/\/\/\----> V+
//                | .     |    
//        <--<|---' .     |   _|_
//                  .     '---o o--------->GND
//
// OR:                         /    e.g. 1k
//                        '---o  o---/\/\/\--->GND
//                        ONLY IF:
//                            output is L/H (NOT L/pulled-up)
//                            and input is Pulled-Up
//                        THIS IS THE CASE with LED_INVERTED.
//
//    (v1.10 listed a resistor between GND and the button. This is
//     unnecessary since low and pulled-up are now the only two states)
//     ("From memory, not verified... check 24scam16"
//       Is it that this was used with a *switch* not a *button*????)
//		With inverter, LED tied high, no resistors: (LED_INVERTED)
//			LED-OFF		(DDR = H-out,	PORT = L)
//			LED-ON/IN	(DDR = L-in,	PORT = H-pull-up)
//				(If IN is tied LOW, the LED will be OFF)
//    NOTE: this will NOT work with a directly-connected (not buffered/
//          inverted) LED tied low
//    NOTE2: Technically, this is the same uC pin-states as Without
//           Inverter, above... EXCEPT adds functionality of a SWITCH *IF*
//           coded differently)


//v2.00-2: These notes taken directly from 1.10 changeLog
//  HAVE NOT BEEN VERIFIED
//      Without Inverter, LED Tied Low (LED_TIED_LOW):
//                      uC Out >--vvv---|>|---->GND
//          LED On      H
//          LED Off     L (Float, with internal pull-ups may light dimly)
//          Float       Value will likely be pulled low through LED, even 
//                      with pull-ups
//                        though value may not be low-enough.
//          Switch: Not possible tied-low... maybe possible tied-high
//               but again LED may not pull low enough.
//       Compatibility: (L/F) will not work. (LF)/H is necessary
//          Status: Unknown
//                
//  Current Status: (Not using H/L or On/Off for states, due to inversion,
//                  etc.)
//          State1: LED_DIRECT_HIGH -> input, pulled-up
//                  LED_INVERTED    -> output, High 
//          State2:
//                  LED_DIRECT_HIGH == LED_INVERTED -> Low, output


//CURRENTLY (prior to v2.00-2):
//  LED_TIED_LOW = LED_INVERTED = TRUE (=LED_BUFFERED)
//  LED_TIED_HIGH = LED_DIRECT_HIGH = FALSE


//NOW FROM CODE:    
// HEART_LEDCONNECTION/  | LED_TIED_HIGH      LED_TIED_LOW
// ledConnection         | LED_DIRECT_HIGH    INVERTED
// ---------------------------------------------------------
//  ledOn = 0            | out, L             out, L
//  ledOn = 1            | in, pu             out, H
   
//!!! THIS DOES NOT MATCH THE NOTES!
//!!! (ledOn is inverted, as well!)

//This is getting too confusing, trying to create names for all the
//possibilities.
// INSTEAD use the PINSTATE macros, as described above.


//_____>>>> SEE before heart_update()




#if 0
//This is how it used to be, in case I ever want to reintroduce variable
//heartPins...
static __inline__
uint8_t heartI_setLED(uint8_t ledOn)
{

	if(ledOn)
	{
#if (!defined(HEARTPIN_HARDCODED) || !HEARTPIN_HARDCODED)
		if(ledConnection == LED_DIRECT_HIGH)
		{
			//Set the direction first, so there's not a short short with the input switch
			//DDR = L,in
			setinVar(heartBeatPin, heartPIN);
			//PORT = H,pull-up
			setpuVar(heartBeatPin, heartPIN);
		}
		else if(ledConnection == LED_INVERTED)
		{
			//Shouldn't be necessary, but might as well...
			setoutVar(heartBeatPin, heartPIN);
			setpinVar(heartBeatPin, heartPIN);
		}
#else
 #if (HEART_LEDCONNECTION == LED_DIRECT_HIGH)
		setinPORT(HEART_PINNUM, HEART_PINPORT);
		setpuPORT(HEART_PINNUM, HEART_PINPORT);
 #elif (HEART_LEDCONNECTION == LED_INVERTED)
		setoutPORT(HEART_PINNUM, HEART_PINPORT);
		setpinPORT(HEART_PINNUM, HEART_PINPORT);
 #else
	#error "Not Handled..."
 #endif
#endif
		return TRUE;
	}
	else
	{
#if (!defined(HEARTPIN_HARDCODED) || !HEARTPIN_HARDCODED)
		//This should be the same for either case...
		
		//Set the port first, so there's not a short 
		//(there may be an instant of floating on the input of an inverter)
		//PORT = L
		clrpinVar(heartBeatPin, heartPIN);
		//DDR = H,out
		setoutVar(heartBeatPin, heartPIN);
#else
		clrpinPORT(HEART_PINNUM, HEART_PINPORT);
		setoutPORT(HEART_PINNUM, HEART_PINPORT);
#endif
		return FALSE;
	}
}
#endif //0



#if (!defined(HEART_INPUTPOLLING_UNUSED) || !HEART_INPUTPOLLING_UNUSED)

uint8_t heart_getButton(void)
{
	return heartButtonVal;
}
#endif


#if 0
//This is how it used to be, should I decide to reinstate variable
//pin-names...
uint8_t heart_getButton(void)
{
	uint8_t pinVal;
	
#if (!defined(HEARTPIN_HARDCODED) || !HEARTPIN_HARDCODED)
	//Make the pin an input, pulled-up
	//OLD:Set the pullup first so it doesn't take so long to raise...
	//Set input first so we don't short the output (PORT=H,pu) to ground
	setinVar(heartBeatPin, heartPIN);
	//PORT = H, pull-up
	setpuVar(heartBeatPin, heartPIN);

	//Insert a delay to allow the pull-up to rise...
	uint8_t delay; 
	for(delay = 0; delay < HEART_PULLUP_DELAY; delay++)
	{  
	   //Necessary or this'll be optimised-out
	   asm("nop;");
	}

	pinVal = getpinVar(heartBeatPin, heartPIN);
#else
	setinPORT(HEART_PINNUM, HEART_PINPORT);
	setpuPORT(HEART_PINNUM, HEART_PINPORT);

	//Insert a delay to allow the pull-up to rise...
	uint8_t delay;
	for(delay = 0; delay < HEART_PULLUP_DELAY; delay++)
	{
		//Necessary or this'll be optimised-out
		asm("nop;");
	}

	pinVal = getpinPORT(HEART_PINNUM, HEART_PINPORT);
#endif

	//Leave the pinstate to be updated with next heart_update
	// to assure valid/safe values of PORT and DDR...
	//Return the pin to the heartbeat
//	setoutVar(heartBeatPin, heartPIN);

	return pinVal;
}
#endif //0
/* mehPL:
 *    I would love to believe in a world where licensing shouldn't be
 *    necessary; where people would respect others' work and wishes, 
 *    and give credit where it's due. 
 *    A world where those who find people's work useful would at least 
 *    send positive vibes--if not an email.
 *    A world where we wouldn't have to think about the potential
 *    legal-loopholes that others may take advantage of.
 *
 *    Until that world exists:
 *
 *    This software and associated hardware design is free to use,
 *    modify, and even redistribute, etc. with only a few exceptions
 *    I've thought-up as-yet (this list may be appended-to, hopefully it
 *    doesn't have to be):
 * 
 *    1) Please do not change/remove this licensing info.
 *       (You can add notes *surrounding* it!)
 *    2) Please do not change/remove others' credit/licensing/copyright 
 *         info, where noted. 
 *    3) If you find yourself profiting from my work, or derivatives,
 *         thereof, please send me a beer, a trinket, or cash is always 
 *         handy as well.
 *         (Please be considerate. E.G. if you've reposted my work on a
 *          revenue-making (ad-based) website, please think of the
 *          years and years of hard work that went into this!)
 *    4) If you/your company *intend(s)* to profit from my work, 
 *         you must get my permission, first. 
 *    5) No permission is given for my work to be used in Military, NSA,
 *         or other creepy-ass purposes. No exceptions. And if there's 
 *         any question in your mind as to whether your project qualifies
 *         under this category, you must get my explicit permission.
 *
 *    The open-sourced project this originated from is ~98% the work of
 *    Me, "Esot.Eric," except where otherwise noted.
 *    That includes the "commonCode" and makefiles.
 *    Thanks, of course, should be given to those who worked on the tools
 *    I've used: avr-dude, avr-gcc, gnu-make, vim, usb-tiny, and 
 *    I'm certain many others. 
 *    And, as well, to the countless coders who've taken time to post
 *    solutions to issues I couldn't solve, all over the internets.
 *
 *
 *    I'd love to hear of how this is being used, suggestions for
 *    improvements, etc!
 *         
 *    The creator of the original code and original hardware can be
 *    contacted at:
 *
 *        EricWazHung At Gmail Dotcom ("Me")
 *
 *    This code's origin (and latest versions) can be found at:
 *          https://github.com/ericwazhung
 *      or: https://github.com/esot-eric-test
 *      or Plausibly under a project page at: 
 *          https://hackaday.io/hacker/40107-esoteric
 *
 *    Older versions may be locatable at: (no longer updated)
 *        https://code.google.com/u/ericwazhung/
 *
 *
 *    The site associated with the original open-sourced project is at:
 *
 *        https://sites.google.com/site/geekattempts/
 *
 *    If any of that ever changes, I will be sure to note it here, 
 *    and add a link at the pages above.
 *
 * This license added to the original file located at:
 * /home/meh/_avrProjects/sdramThingZero/21-gittifying/_commonCode_localized/heartbeat/2.06/heartbeat.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *    --Esot.Eric (aka EricWazhung)
 */
