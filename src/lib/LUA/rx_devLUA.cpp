#ifdef TARGET_RX

#include "common.h"
#include "device.h"

#include "CRSF.h"
#include "POWERMGNT.h"
#include "config.h"
#include "logging.h"
#include "lua.h"
#include "OTA.h"
#include "hwTimer.h"
#include "FHSS.h"

extern void deferExecution(uint32_t ms, std::function<void()> f);

extern bool InLoanBindingMode;
extern bool returnModelFromLoan;

static char modelString[] = "000";

#if defined(POWER_OUTPUT_VALUES)
static struct luaItem_selection luaTlmPower = {
    {"Tlm Power", CRSF_TEXT_SELECTION},
    0, // value
    strPowerLevels,
    "mW"
};
#endif

#if defined(GPIO_PIN_ANTENNA_SELECT)
static struct luaItem_selection luaAntennaMode = {
    {"Ant. Mode", CRSF_TEXT_SELECTION},
    0, // value
    "Antenna B;Antenna A;Diversity",
    emptySpace
};
#endif

//----------------------------Info-----------------------------------

static struct luaItem_string luaModelNumber = {
    {"Model Id", CRSF_INFO},
    modelString
};

static struct luaItem_string luaELRSversion = {
    {version, CRSF_INFO},
    commit
};

//----------------------------Info-----------------------------------

//---------------------------- WiFi -----------------------------


//---------------------------- WiFi -----------------------------

//---------------------------- Model Loan Out -----------------------------

static struct luaItem_command luaLoanModel = {
    {"Loan Model", CRSF_COMMAND},
    lcsIdle, // step
    emptySpace
};

static struct luaItem_command luaReturnModel = {
    {"Return Model", CRSF_COMMAND},
    lcsIdle, // step
    emptySpace
};

//---------------------------- Model Loan Out -----------------------------


static void registerLuaParameters()
{

  if (GPIO_PIN_ANTENNA_SELECT != UNDEF_PIN)
  {
    registerLUAParameter(&luaAntennaMode, [](struct luaPropertiesCommon* item, uint8_t arg){
      config.SetAntennaMode(arg);
    });
  }
#if defined(POWER_OUTPUT_VALUES)
  luadevGeneratePowerOpts(&luaTlmPower);
  registerLUAParameter(&luaTlmPower, [](struct luaPropertiesCommon* item, uint8_t arg){
    config.SetPower(arg);
    POWERMGNT::setPower((PowerLevels_e)constrain(arg + MinPower, MinPower, MaxPower));
  });
#endif
  registerLUAParameter(&luaLoanModel, [](struct luaPropertiesCommon* item, uint8_t arg){
    // Do it when polling for status i.e. going back to idle, because we're going to lose conenction to the TX
    if (arg == 6) {
      deferExecution(200, [](){ InLoanBindingMode = true; });
    }
    sendLuaCommandResponse(&luaLoanModel, arg < 5 ? lcsExecuting : lcsIdle, arg < 5 ? "Sending..." : "");
  });
  registerLUAParameter(&luaReturnModel, [](struct luaPropertiesCommon* item, uint8_t arg){
    // Do it when polling for status i.e. going back to idle, because we're going to lose conenction to the TX
    if (arg == 6) {
      deferExecution(200, []() { returnModelFromLoan = true; });
    }
    sendLuaCommandResponse(&luaReturnModel, arg < 5 ? lcsExecuting : lcsIdle, arg < 5 ? "Sending..." : "");
  });

  registerLUAParameter(&luaModelNumber);
  registerLUAParameter(&luaELRSversion);
  registerLUAParameter(NULL);
}

static int event()
{

  if (GPIO_PIN_ANTENNA_SELECT != UNDEF_PIN)
  {
    setLuaTextSelectionValue(&luaAntennaMode, config.GetAntennaMode());
  }

#if defined(POWER_OUTPUT_VALUES)
  setLuaTextSelectionValue(&luaTlmPower, config.GetPower());
#endif

  if (config.GetModelId() == 255)
  {
    setLuaStringValue(&luaModelNumber, "Off");
  }
  else
  {
    itoa(config.GetModelId(), modelString, 10);
    setLuaStringValue(&luaModelNumber, modelString);
  }
  return DURATION_IMMEDIATELY;
}

static int timeout()
{
  luaHandleUpdateParameter();
  return DURATION_IMMEDIATELY;
}

static int start()
{
  registerLuaParameters();
  event();
  return DURATION_IMMEDIATELY;
}

device_t LUA_device = {
  .initialize = NULL,
  .start = start,
  .event = event,
  .timeout = timeout
};

#endif
