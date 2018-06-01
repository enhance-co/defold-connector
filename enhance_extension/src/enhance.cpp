#define LIB_NAME "enhance"
#define MODULE_NAME "enhance"

#include "enhance_internal.h"
#include <stdio.h>
#include <string.h>

// LUA<=>C++ functions for Enhance

EnhanceListener g_interstitialCompleted, g_currencyGranted;
EnhanceListener g_rewardGranted, g_rewardDeclined, g_rewardUnavailable;
EnhanceListener g_permissionGranted, g_permissionRefused;
EnhanceListener g_onPurchaseSuccess, g_onPurchaseFailed;
EnhanceListener g_onConsumeSuccess, g_onConsumeFailed;
EnhanceListener g_onRestoreSuccess, g_onRestoreFailed;
EnhanceListener g_onServiceOptInRequirement;
EnhanceListener g_onDialogComplete;

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
	bool bIsReady = false;

	if (lua_gettop(L) == 1) {
   		const char* placement = luaL_checkstring(L, 1);
  
	 	bIsReady = Defold_Enhance_isInterstitialReady(placement);
   	}
   	else{
    	bIsReady = Defold_Enhance_isInterstitialReady("default");
    }
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceShowInterstitial(lua_State* L)
{
	if (lua_gettop(L) == 1) {
   		const char* placement = luaL_checkstring(L, 1);
  
   		Defold_Enhance_showInterstitial(placement);
   	}
   	else{
    	Defold_Enhance_showInterstitial("default");
    }
   
   // Return no value
   return 0;
}

static int EnhanceIsRewardedAdReady(lua_State* L)
{
	bool bIsReady = false;
	
	if (lua_gettop(L) == 1) {
   		const char* placement = luaL_checkstring(L, 1);
   
   		bIsReady = Defold_Enhance_isRewardedAdReady(placement);
   	}
   	else
   	{
   		bIsReady = Defold_Enhance_isRewardedAdReady("neutral");
   	}
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceShowRewardedAd(lua_State* L)
{
	if (lua_gettop(L) == 3) {
   		g_rewardGranted.set(L, 1);
   		g_rewardDeclined.set(L, 2);
   		g_rewardUnavailable.set(L, 3);
   
   		Defold_Enhance_showRewardedAd("neutral", &g_rewardGranted, &g_rewardDeclined, &g_rewardUnavailable);
   }else if(lua_gettop(L) == 4) {
   		const char* placement = luaL_checkstring(L, 1);
   		
   		g_rewardGranted.set(L, 2);
   		g_rewardDeclined.set(L, 3);
   		g_rewardUnavailable.set(L, 4);
   		
   		Defold_Enhance_showRewardedAd(placement, &g_rewardGranted, &g_rewardDeclined, &g_rewardUnavailable);
   }
   
   // Return no value
   return 0;
}

static int EnhanceIsOfferwallReady(lua_State* L)
{
	bool bIsReady = false;

	if (lua_gettop(L) == 1) {
   		const char* placement = luaL_checkstring(L, 1);
  
	 	bIsReady = Defold_Enhance_isOfferwallReady(placement);
   	}
   	else{
    	bIsReady = Defold_Enhance_isOfferwallReady("default");
    }
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceShowOfferwall(lua_State* L)
{
	if (lua_gettop(L) == 1) {
   		const char* placement = luaL_checkstring(L, 1);

   		Defold_Enhance_showOfferwall(placement);
   	}else{
   		Defold_Enhance_showOfferwall("default");
   	}
   
   // Return no value
   return 0;
}

static int EnhanceIsSpecialOfferReady(lua_State* L)
{
	bool bIsReady = false;

	if (lua_gettop(L) == 1) {
   		const char* placement = luaL_checkstring(L, 1);
  
	 	bIsReady = Defold_Enhance_isSpecialOfferReady(placement);
   	}
   	else{
    	bIsReady = Defold_Enhance_isSpecialOfferReady("default");
    }
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceShowSpecialOffer(lua_State* L)
{
	if (lua_gettop(L) == 1) {
   		const char* placement = luaL_checkstring(L, 1);

   		Defold_Enhance_showSpecialOffer(placement);
   	}else{
   		Defold_Enhance_showSpecialOffer("default");
   	}
   
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

static int EnhanceIsBannerAdReady(lua_State* L)
{
	bool bIsReady = false;

	if (lua_gettop(L) == 1) {
   		const char* placement = luaL_checkstring(L, 1);
  
	 	bIsReady = Defold_Enhance_isBannerAdReady(placement);
   	}
   	else{
    	bIsReady = Defold_Enhance_isBannerAdReady("default");
    }
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceShowBannerAd(lua_State* L)
{
	const char* position;
	const char* placement;
	
	if (lua_gettop(L) == 1) {
		position = luaL_checkstring(L, 1);
		Defold_Enhance_showBannerAd("default", position);
	}else if (lua_gettop(L) == 2) {
		placement = luaL_checkstring(L, 1);
		position = luaL_checkstring(L, 2);
		Defold_Enhance_showBannerAd(placement, position);
	}else{
		Defold_Enhance_showBannerAd("default", "bottom");
	}
   
   // Return no value
   return 0;
}

static int EnhanceHideBannerAd(lua_State* L)
{
   Defold_Enhance_hideBannerAd();
   
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

static int EnhanceIAPIsSupported(lua_State* L)
{
   bool bIsReady = Defold_EnhanceInAppPurchases_isSupported();
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceAttemptPurchase(lua_State* L)
{

   const char* str_sku = luaL_checkstring(L, 1);

   g_onPurchaseSuccess.set(L, 2);
   g_onPurchaseFailed.set(L, 3);
   
   Defold_EnhanceInAppPurchases_attemptPurchase(str_sku, &g_onPurchaseSuccess, &g_onPurchaseFailed);
   
   // Return no value
   return 0;
}


static int EnhanceConsume(lua_State* L)
{

   const char* str_sku = luaL_checkstring(L, 1);

   g_onConsumeSuccess.set(L, 2);
   g_onConsumeFailed.set(L, 3);
   
   Defold_EnhanceInAppPurchases_consume(str_sku, &g_onConsumeSuccess, &g_onConsumeFailed);
   
   // Return no value
   return 0;
}

static int EnhanceGetDisplayPrice(lua_State* L)
{
   const char* str_sku = luaL_checkstring(L, 1);
   const char* str_default_price = luaL_checkstring(L, 2);
   
   const char* price_str = Defold_EnhanceInAppPurchases_getDisplayPrice(str_sku, str_default_price);
   
   // Return 1 value
   lua_pushstring(L, price_str);
   return 1;
}

static int EnhanceIsItemOwned(lua_State* L)
{
	const char* str_sku = luaL_checkstring(L, 1);

   bool bIsReady = Defold_EnhanceInAppPurchases_isItemOwned(str_sku);
   
   // Return 1 value
   lua_pushboolean(L, bIsReady);
   return 1;
}

static int EnhanceGetOwnedItemCount(lua_State* L)
{
	const char* str_sku = luaL_checkstring(L, 1);

   int itemCount = Defold_EnhanceInAppPurchases_getOwnedItemCount(str_sku);
   
   // Return 1 value
   lua_pushinteger(L, itemCount);
   return 1;
}

static int EnhanceManuallyRestorePurchases(lua_State* L)
{
   g_onRestoreSuccess.set(L, 1);
   g_onRestoreFailed.set(L, 2);

   Defold_EnhanceInAppPurchases_manuallyRestorePurchases(&g_onRestoreSuccess, &g_onRestoreFailed);

   // ret no value
   return 0;
}

static int EnhanceGetDisplayTitle(lua_State* L)
{
   const char* str_sku = luaL_checkstring(L, 1);
   const char* str_default_title = luaL_checkstring(L, 2);

   const char* title_str = Defold_EnhanceInAppPurchases_getDisplayTitle(str_sku, str_default_title);

   lua_pushstring(L, title_str);
   return 1;
}

static int EnhanceGetDisplayDescription(lua_State* L)
{
   const char* str_sku = luaL_checkstring(L, 1);
   const char* str_default_description = luaL_checkstring(L, 2);

   const char* description_str = Defold_EnhanceInAppPurchases_getDisplayDescription(str_sku, str_default_description);

   lua_pushstring(L, description_str);
   return 1;
}

static int EnhanceRequiresDataConsentOptIn(lua_State* L)
{
    g_onServiceOptInRequirement.set(L, 1);

    Defold_Enhance_requiresDataConsentOptIn(&g_onServiceOptInRequirement);
    return 0;
}

static int EnhanceServiceTermsOptIn(lua_State* L)
{
    char buffer[1024] = "";
    int n = lua_gettop(L);
    int i;

    for(i = 1; i <= n; i++) {
        if(i > 1) {
            strcat(buffer, ",");
        }

        const char *str_sdk = luaL_checkstring(L, i);
        strcat(buffer, str_sdk);
    }

    Defold_Enhance_serviceTermsOptIn(buffer);
    return 0;
}

static int EnhanceShowServiceOptInDialogs(lua_State* L)
{
    g_onDialogComplete.set(L, 1);

    char buffer[1024] = "";
    int n = lua_gettop(L);
    int i;

    for(i = 2; i <= n; i++) {
        if(i > 2) {
            strcat(buffer, ",");
        }

        const char *str_sdk = luaL_checkstring(L, i);
        strcat(buffer, str_sdk);
    }

    Defold_Enhance_showServiceOptInDialogs(buffer, &g_onDialogComplete);
    return 0;
}

static int EnhanceServiceTermsOptOut(lua_State* L)
{
    Defold_Enhance_serviceTermsOptOut();
    return 0;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
   {"isEnhanced", EnhanceIsEnhanced},
   {"setInterstitialCallback", EnhanceSetInterstitialCallback},
   {"setCurrencyCallback", EnhanceSetCurrencyCallback},
   {"setReceivedCurrencyCallback", EnhanceSetCurrencyCallback},
   {"isInterstitialReady", EnhanceIsInterstitialReady},
   {"showInterstitial", EnhanceShowInterstitial},
   {"showInterstitialAd", EnhanceShowInterstitial},
   {"isRewardedAdReady", EnhanceIsRewardedAdReady},
   {"showRewardedAd", EnhanceShowRewardedAd},
   {"isOfferwallReady", EnhanceIsOfferwallReady},
   {"showOfferwall", EnhanceShowOfferwall},
   {"isSpecialOfferReady", EnhanceIsSpecialOfferReady},
   {"showSpecialOffer", EnhanceShowSpecialOffer},
   {"isBannerAdReady", EnhanceIsBannerAdReady},
   {"showBannerAd", EnhanceShowBannerAd},
   {"showBannerAdWithPosition", EnhanceShowBannerAd},
   {"hideBannerAd", EnhanceHideBannerAd},
   {"isFullscreenAdShowing", EnhanceIsFullscreenAdShowing},
   {"logCustomEvent", EnhanceLogCustomEvent},
   {"logEvent", EnhanceLogCustomEvent},
   {"requestLocalNotificationPermission", EnhanceRequestLocalNotificationPermission},
   {"enableLocalNotification", EnhanceEnableLocalNotification},
   {"disableLocalNotification", EnhanceDisableLocalNotification},
   {"iapIsSupported", EnhanceIAPIsSupported},
   {"attemptPurchase", EnhanceAttemptPurchase},
   {"consume", EnhanceConsume},   
   {"getDisplayPrice", EnhanceGetDisplayPrice},
   {"isItemOwned", EnhanceIsItemOwned},
   {"getOwnedItemCount", EnhanceGetOwnedItemCount},
   {"manuallyRestorePurchases", EnhanceManuallyRestorePurchases},
   {"getDisplayTitle", EnhanceGetDisplayTitle},
   {"getDisplayDescription", EnhanceGetDisplayDescription},
   {"serviceTermsOptIn", EnhanceServiceTermsOptIn},
   {"requiresDataConsentOptIn", EnhanceRequiresDataConsentOptIn},
   {"showServiceOptInDialogs", EnhanceShowServiceOptInDialogs},
   {"serviceTermsOptOut", EnhanceServiceTermsOptOut},
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

dmExtension::Result UpdateExtension(dmExtension::Params* params)
{
	Defold_Enhance_pumpEvents();
    return dmExtension::RESULT_OK;
}


// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)

DM_DECLARE_EXTENSION(EnhanceExtension, LIB_NAME, AppInitializeEnhanceExtension, AppFinalizeEnhanceExtension, InitializeEnhanceExtension, UpdateExtension, 0, FinalizeEnhanceExtension)
