#ifndef _ENHANCE_INTERNAL_H
#define _ENHANCE_INTERNAL_H

#include <dmsdk/sdk.h>

class EnhanceListener {
public:
   EnhanceListener() :
         m_L (NULL),
         m_Callback(LUA_NOREF),
         m_Self(LUA_NOREF) {
   }
   
   void set(lua_State *L, int paramIdx) {
      if (!lua_isfunction(L, paramIdx)) {
         dmLogError("Enhance: parameter %d is not a Lua function\n", paramIdx);
      }
      else {         
         lua_pushvalue(L, paramIdx);
         int cb = dmScript::Ref(L, LUA_REGISTRYINDEX);
         
         unset();
         
         m_L = dmScript::GetMainThread(L);
         m_Callback = cb;
         dmScript::GetInstance(L);
         m_Self = dmScript::Ref(L, LUA_REGISTRYINDEX);
      }
   }
   
   void unset() {
      if (m_Callback != LUA_NOREF) {
         dmScript::Unref(m_L, LUA_REGISTRYINDEX, m_Callback);
         dmScript::Unref(m_L, LUA_REGISTRYINDEX, m_Self);
         m_L = NULL;
         m_Callback = LUA_NOREF;
         m_Self = LUA_NOREF;
      }
   }
   
   void callWithNoParam() {
      int top = lua_gettop(m_L);
      
      lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_Callback);
      
      lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_Self);
      lua_pushvalue(m_L, -1);
      dmScript::SetInstance(m_L);
      
      int ret = lua_pcall(m_L, 1, LUA_MULTRET, 0);
      if (ret != 0) {
         dmLogError("Enhance: error calling listener: %s", lua_tostring(m_L, -1));
         lua_pop(m_L, 1);
      }
      
      assert(top == lua_gettop(m_L));
   }
   
   void callWithStrParam(const char *pParam1) {
      int top = lua_gettop(m_L);
      
      lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_Callback);
      
      lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_Self);
      lua_pushvalue(m_L, -1);
      dmScript::SetInstance(m_L);
      
      lua_pushstring(m_L, pParam1);
      
      int ret = lua_pcall(m_L, 2, LUA_MULTRET, 0);
      if (ret != 0) {
         dmLogError("Enhance: error calling listener: %s", lua_tostring(m_L, -1));
         lua_pop(m_L, 1);
      }
      
      assert(top == lua_gettop(m_L));
   }
   
   void callWithIntParam(int nParam1) {
      int top = lua_gettop(m_L);
      
      lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_Callback);
      
      lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_Self);
      lua_pushvalue(m_L, -1);
      dmScript::SetInstance(m_L);
      
      lua_pushinteger(m_L, nParam1);
      
      int ret = lua_pcall(m_L, 2, LUA_MULTRET, 0);
      if (ret != 0) {
         dmLogError("Enhance: error calling listener: %s", lua_tostring(m_L, -1));
         lua_pop(m_L, 1);
      }
      
      assert(top == lua_gettop(m_L));
   }
   
   void callWithIntStrParam(int nParam1, const char *pParam2) {
      int top = lua_gettop(m_L);
      
      lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_Callback);
      
      lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_Self);
      lua_pushvalue(m_L, -1);
      dmScript::SetInstance(m_L);
      
      lua_pushinteger(m_L, nParam1);
      lua_pushstring(m_L, pParam2);
      
      int ret = lua_pcall(m_L, 3, LUA_MULTRET, 0);
      if (ret != 0) {
         dmLogError("Enhance: error calling listener: %s", lua_tostring(m_L, -1));
         lua_pop(m_L, 1);
      }
      
      assert(top == lua_gettop(m_L));
   }
   
   lua_State *m_L;
   int m_Callback;
   int m_Self;
};

// Defined in the defold enhance glue lib for the platform
extern "C" {
   void Defold_Enhance_Initialize();
   bool Defold_Enhance_isEnhanced();
   void Defold_Enhance_setInterstitialCallback(EnhanceListener *pCompleted);
   void Defold_Enhance_setCurrencyCallback(EnhanceListener *pGranted);
   bool Defold_Enhance_isInterstitialReady(const char *pszPlacement);
   void Defold_Enhance_showInterstitial(const char *pszPlacement);
   bool Defold_Enhance_isRewardedAdReady(const char *pszPlacement);
   void Defold_Enhance_showRewardedAd(const char *pszPlacement, EnhanceListener *pGranted, EnhanceListener *pDeclined, EnhanceListener *pUnavailable);
   bool Defold_Enhance_isOfferwallReady(const char *pszPlacement);
   void Defold_Enhance_showOfferwall(const char *pszPlacement);
   bool Defold_Enhance_isSpecialOfferReady(const char *pszPlacement);
   void Defold_Enhance_showSpecialOffer(const char *pszPlacement);
   bool Defold_Enhance_isBannerAdReady(const char *pszPlacement);
   bool Defold_Enhance_isFullscreenAdShowing();
   void Defold_Enhance_showBannerAd(const char *pszPlacement, const char *pszPosition);
   void Defold_Enhance_hideBannerAd();
   void Defold_Enhance_logCustomEvent(const char *pszEventName, const char *pszParamKey, const char *pszParamValue);
   void Defold_Enhance_requestLocalNotificationPermission(EnhanceListener *pGranted, EnhanceListener *pRefused);
   void Defold_Enhance_enableLocalNotification(const char *pszTitle, const char *pszMessage, int delay);
   void Defold_Enhance_disableLocalNotification();
   void Defold_Enhance_pumpEvents();
   bool Defold_EnhanceInAppPurchases_isSupported();
   void Defold_EnhanceInAppPurchases_attemptPurchase(const char *str_sku, EnhanceListener *callback_onPurchaseSuccess, EnhanceListener *callback_onPurchaseFailed);
   void Defold_EnhanceInAppPurchases_consume(const char *str_sku, EnhanceListener *callback_onConsumeSuccess, EnhanceListener *callback_onConsumeFailed);
   const char* Defold_EnhanceInAppPurchases_getDisplayPrice(const char *str_sku, const char *str_default_price);
   bool Defold_EnhanceInAppPurchases_isItemOwned(const char *str_sku);
   int Defold_EnhanceInAppPurchases_getOwnedItemCount(const char *str_sku);
   void Defold_EnhanceInAppPurchases_manuallyRestorePurchases(EnhanceListener *callback_onRestoreSuccess, EnhanceListener *callback_onRestoreFailed);
   const char* Defold_EnhanceInAppPurchases_getDisplayTitle(const char *str_sku, const char *str_default_title);
   const char* Defold_EnhanceInAppPurchases_getDisplayDescription(const char *str_sku, const char *str_default_description);
   // GDPR
   void Defold_Enhance_requiresDataConsentOptIn(EnhanceListener *callback_onServiceOptInRequirement);
   void Defold_Enhance_serviceTermsOptIn(const char *str_sdks);
   void Defold_Enhance_showServiceOptInDialogs(const char *str_sdks, EnhanceListener *callback_onDialogComplete);
   void Defold_Enhance_serviceTermsOptOut();
}

#endif /* _ENHANCE_INTERNAL_H */
