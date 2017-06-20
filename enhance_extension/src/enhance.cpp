#define LIB_NAME "EnhanceExtension"
#define MODULE_NAME "enhance_extension"

#include "enhance_internal.h"

// LUA<=>C++ functions for Enhance

EnhanceListener g_interstitialCompleted, g_currencyGranted;
EnhanceListener g_rewardGranted, g_rewardDeclined, g_rewardUnavailable;
EnhanceListener g_permissionGranted, g_permissionRefused;

static int EnhanceIsEnhanced(lua_State* L)
{
   bool bIsReady = Defold_Enhance_isEnhanced();
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceSetInterstitialCallback(lua_State* L)
{
   g_interstitialCompleted.set(L, 1);
   
   Defold_Enhance_setInterstitialCallback(&g_interstitialCompleted);
   
   // Return no value
   return 0;
}

static int EnhanceSetCurrencyCallback(lua_State* L)
{
   g_currencyGranted.set(L, 1);
   
   Defold_Enhance_setCurrencyCallback(&g_currencyGranted);
   
   // Return no value
   return 0;
}

static int EnhanceIsInterstitialReady(lua_State* L)
{
   const char* placement = luaL_checkstring(L, 1);
   
   if (placement == NULL) placement = "default";
   
   bool bIsReady = Defold_Enhance_isInterstitialReady(placement);
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceShowInterstitial(lua_State* L)
{
   const char* placement = luaL_checkstring(L, 1);
   
   if (placement == NULL) placement = "default";
   
   Defold_Enhance_showInterstitial(placement);
   
   // Return no value
   return 0;
}

static int EnhanceIsRewardedAdReady(lua_State* L)
{
   const char* placement = luaL_checkstring(L, 1);
   
   if (placement == NULL) placement = "default";
   
   bool bIsReady = Defold_Enhance_isRewardedAdReady(placement);
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceShowRewardedAd(lua_State* L)
{
   const char* placement = luaL_checkstring(L, 1);
   
   if (placement == NULL) placement = "default";
   
   fprintf(stderr, "EnhanceShowRewardedAd: %s\n", placement);
   
   g_rewardGranted.set(L, 2);
   g_rewardDeclined.set(L, 3);
   g_rewardUnavailable.set(L, 4);
   
   Defold_Enhance_showRewardedAd(placement, &g_rewardGranted, &g_rewardDeclined, &g_rewardUnavailable);
   
   // Return no value
   return 0;
}

static int EnhanceIsOfferwallReady(lua_State* L)
{
   bool bIsReady = Defold_Enhance_isOfferwallReady();
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceShowOfferwall(lua_State* L)
{
   Defold_Enhance_showOfferwall();
   
   // Return no value
   return 0;
}

static int EnhanceIsSpecialOfferReady(lua_State* L)
{
   bool bIsReady = Defold_Enhance_isSpecialOfferReady();
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceShowSpecialOffer(lua_State* L)
{
   Defold_Enhance_showSpecialOffer();
   
   // Return no value
   return 0;
}

static int EnhanceIsFullscreenAdShowing(lua_State* L)
{
   bool bIsReady = Defold_Enhance_isFullscreenAdShowing();
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceIsOverlayAdReady(lua_State* L)
{
   bool bIsReady = Defold_Enhance_isOverlayAdReady();
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceShowOverlayAd(lua_State* L)
{
   const char* position = luaL_checkstring(L, 1);
   
   if (position == NULL) position = "top";
   
   Defold_Enhance_showOverlayAd(position);
   
   // Return no value
   return 0;
}

static int EnhanceHideOverlayAd(lua_State* L)
{
   Defold_Enhance_hideOverlayAd();
   
   // Return no value
   return 0;
}

static int EnhanceLogCustomEvent(lua_State* L)
{
   const char* eventName = luaL_checkstring(L, 1);
   const char* paramKey = luaL_optstring(L, 2, NULL);
   const char* paramValue = luaL_optstring(L, 3, NULL);
   
   if (eventName)
      Defold_Enhance_logCustomEvent(eventName, paramKey, paramValue);
   
   // Return no value
   return 0;
}

static int EnhanceRequestLocalNotificationPermission(lua_State* L)
{
   g_permissionGranted.set(L, 1);
   g_permissionRefused.set(L, 2);
   
   Defold_Enhance_requestLocalNotificationPermission(&g_permissionGranted, &g_permissionRefused);
   
   // Return no value
   return 0;
}

static int EnhanceEnableLocalNotification(lua_State* L)
{
   const char* title = luaL_checkstring(L, 1);
   const char* message = luaL_checkstring(L, 2);
   int delay = luaL_checkinteger(L, 3);
   
   Defold_Enhance_enableLocalNotification(title, message, delay);
   
   // Return no value
   return 0;
}

static int EnhanceDisableLocalNotification(lua_State* L)
{
   Defold_Enhance_disableLocalNotification();
   
   // Return no value
   return 0;
}

static int EnhancePumpEvents(lua_State* L)
{
   Defold_Enhance_pumpEvents();
   
   // Return no value
   return 0;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
   {"isEnhanced", EnhanceIsEnhanced},
   {"setInterstitialCallback", EnhanceSetInterstitialCallback},
   {"setCurrencyCallback", EnhanceSetCurrencyCallback},
   {"isInterstitialReady", EnhanceIsInterstitialReady},
   {"showInterstitial", EnhanceShowInterstitial},
   {"isRewardedAdReady", EnhanceIsRewardedAdReady},
   {"showRewardedAd", EnhanceShowRewardedAd},
   {"isOfferwallReady", EnhanceIsOfferwallReady},
   {"showOfferwall", EnhanceShowOfferwall},
   {"isSpecialOfferReady", EnhanceIsSpecialOfferReady},
   {"showSpecialOffer", EnhanceShowSpecialOffer},
   {"isOverlayAdReady", EnhanceIsOverlayAdReady},
   {"showOverlayAd", EnhanceShowOverlayAd},
   {"hideOverlayAd", EnhanceHideOverlayAd},
   {"isFullscreenAdShowing", EnhanceIsFullscreenAdShowing},
   {"logCustomEvent", EnhanceLogCustomEvent},
   {"requestLocalNotificationPermission", EnhanceRequestLocalNotificationPermission},
   {"enableLocalNotification", EnhanceEnableLocalNotification},
   {"disableLocalNotification", EnhanceDisableLocalNotification},
   {"pumpEvents", EnhancePumpEvents},
   {0, 0}
};

static void LuaInit(lua_State* L)
{
   int top = lua_gettop(L);
   
   // Register lua names
   luaL_register(L, MODULE_NAME, Module_methods);
   
   lua_pop(L, 1);
   assert(top == lua_gettop(L));
}

dmExtension::Result AppInitializeEnhanceExtension(dmExtension::AppParams* params)
{
   return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeEnhanceExtension(dmExtension::Params* params)
{
   // Init Lua
   LuaInit(params->m_L);
   Defold_Enhance_Initialize();
   printf("Registered %s Extension\n", MODULE_NAME);
   return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeEnhanceExtension(dmExtension::AppParams* params)
{
   return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeEnhanceExtension(dmExtension::Params* params)
{
   return dmExtension::RESULT_OK;
}


// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)

DM_DECLARE_EXTENSION(EnhanceExtension, LIB_NAME, AppInitializeEnhanceExtension, AppFinalizeEnhanceExtension, InitializeEnhanceExtension, 0, 0, FinalizeEnhanceExtension)
