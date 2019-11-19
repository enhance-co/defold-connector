#if defined(DM_PLATFORM_IOS)

#include "enhance_internal.h"
#include "FglEnhance.h"
#include "FglEnhanceInAppPurchases.h"
#include "Enhance.h"
#include "EnhanceInAppPurchases.h"

static EnhanceListener *g_pInterstitialCompleted;
static EnhanceListener *g_pCurrencyGranted;
static EnhanceListener *g_pRewardGranted;
static EnhanceListener *g_pRewardDeclined;
static EnhanceListener *g_pRewardUnavailable;
static EnhanceListener *g_pPermissionGranted;
static EnhanceListener *g_pPermissionRefused;
static EnhanceListener *g_pPurchaseSuccess;
static EnhanceListener *g_pPurchaseFailed;
static EnhanceListener *g_pRestoreSuccess;
static EnhanceListener *g_pRestoreFailed;
static EnhanceListener *g_pConsumeSuccess;
static EnhanceListener *g_pConsumeFailed;
static EnhanceListener *g_pOnServiceOptInRequirement;
static EnhanceListener *g_pOnDialogComplete;


@interface DefoldEnhanceDelegate : NSObject<InterstitialDelegate, RewardDelegate, CurrencyGrantedDelegate, PermissionDelegate, RestoreDelegate, ConsumeDelegate, DataConsentDelegate> {
}
@end

@implementation DefoldEnhanceDelegate;

-(void)onInterstitialCompleted {
   dmLogInfo("EnhanceDefold[iOS]: onInterstitialCompleted");
   if (g_pInterstitialCompleted)
      g_pInterstitialCompleted->callWithNoParam();
}

-(void)onCurrencyGranted:(int)amount {
   dmLogInfo("EnhanceDefold[iOS]: onCurrencyGranted: %d", amount);
   if (g_pCurrencyGranted)
      g_pCurrencyGranted->callWithIntParam(amount);
}

-(void)onRewardGranted:(int)rewardValue rewardType:(RewardType)rewardType {
   dmLogInfo("EnhanceDefold[iOS]: onRewardGranted");
   if (g_pRewardGranted) {
      if (rewardType == REWARDTYPE_COINS)
         g_pRewardGranted->callWithIntStrParam(rewardValue, "REWARDTYPE_COINS");
      else
         g_pRewardGranted->callWithIntStrParam(0, "REWARDTYPE_ITEM");
      
   }
}

-(void)onRewardDeclined {
   dmLogInfo("EnhanceDefold[iOS]: onRewardDeclined");
   if (g_pRewardDeclined) {
      g_pRewardDeclined->callWithNoParam();
   }
}

-(void)onRewardUnavailable {
   dmLogInfo("EnhanceDefold[iOS]: onRewardUnavailable");
   if (g_pRewardUnavailable) {
      g_pRewardUnavailable->callWithNoParam();
   }
}

-(void)onPermissionGranted {
   dmLogInfo("EnhanceDefold[iOS]: onPermissionGranted");
   if (g_pPermissionGranted) {
      g_pPermissionGranted->callWithNoParam();
   }
}

-(void)onPermissionRefused {
   dmLogInfo("EnhanceDefold[iOS]: onPermissionRefused");
   if (g_pPermissionRefused) {
      g_pPermissionRefused->callWithNoParam();
   }
}

-(void)onPurchaseSuccess {
   dmLogInfo("EnhanceDefold[iOS]: onPurchaseSuccess");
   if (g_pPurchaseSuccess) {
      g_pPurchaseSuccess->callWithNoParam();
   }
}
-(void)onPurchaseFailed {
   dmLogInfo("EnhanceDefold[iOS]: onPurchaseFailed");
   if (g_pPurchaseFailed) {
      g_pPurchaseFailed->callWithNoParam();
   }
}
-(void)onRestoreSuccess {
   dmLogInfo("EnhanceDefold[iOS]: onRestoreSuccess");
   if (g_pRestoreSuccess) {
      g_pRestoreSuccess->callWithNoParam();
   }
}
-(void)onRestoreFailed {
   dmLogInfo("EnhanceDefold[iOS]: onRestoreFailed");
   if (g_pRestoreFailed) {
      g_pRestoreFailed->callWithNoParam();
   }
}

-(void)onConsumeSuccess {
   dmLogInfo("EnhanceDefold[iOS]: onConsumeSuccess");
   if(g_pConsumeSuccess) {
      g_pConsumeSuccess->callWithNoParam();
   }
}

-(void)onConsumeFailed {
   dmLogInfo("EnhanceDefold[iOS]: onConsumeFailed");
   if(g_pConsumeFailed) {
      g_pConsumeFailed->callWithNoParam();
   }
}

-(void)onServiceOptInRequirement:(BOOL)isUserOptInRequired {
    dmLogInfo("EnhanceDefold[iOS]: onServiceOptInRequirement");
    if(g_pOnServiceOptInRequirement) {
        g_pOnServiceOptInRequirement->callWithIntParam((isUserOptInRequired ? 1 : 0));
    }
}

-(void)onOptInDialogComplete {
    dmLogInfo("EnhanceDefold[iOS]: onOptInDialogComplete");
    if(g_pOnDialogComplete) {
        g_pOnDialogComplete->callWithNoParam();
    }
}

@end

static DefoldEnhanceDelegate *g_delegate;

void Defold_Enhance_Initialize() {
   dmLogInfo("EnhanceDefold[iOS]: initialize");
   g_delegate = [[DefoldEnhanceDelegate alloc] init];
}

bool Defold_Enhance_isEnhanced() {
   return [Enhance isEnhanced];
}

void Defold_Enhance_setInterstitialCallback(EnhanceListener *pCompleted) {
   g_pInterstitialCompleted = pCompleted;
}

void Defold_Enhance_setCurrencyCallback(EnhanceListener *pGranted) {
   g_pCurrencyGranted = pGranted;
}

bool Defold_Enhance_isInterstitialReady(const char *pszPlacement) {
   NSString *placement = [NSString stringWithUTF8String:pszPlacement];
   
   return [Enhance isInterstitialReady:placement];
}

void Defold_Enhance_showInterstitial(const char *pszPlacement) {
   NSString *placement = [NSString stringWithUTF8String:pszPlacement];
   
   [[FglEnhance sharedInstance] setInterstitialDelegate:g_delegate];
   
   dmLogInfo("EnhanceDefold[iOS]: show interstitial");
   [Enhance showInterstitialAd:placement];
}

bool Defold_Enhance_isRewardedAdReady(const char *pszPlacement) {
   NSString *placement = [NSString stringWithUTF8String:pszPlacement];
   
   return [Enhance isRewardedAdReady:placement];
}

void Defold_Enhance_showRewardedAd(const char *pszPlacement, EnhanceListener *pGranted, EnhanceListener *pDeclined, EnhanceListener *pUnavailable) {
   NSString *placement = [NSString stringWithUTF8String:pszPlacement];
   
   g_pRewardGranted = pGranted;
   g_pRewardDeclined = pDeclined;
   g_pRewardUnavailable = pUnavailable;
   
   dmLogInfo("EnhanceDefold[iOS]: show rewarded ad");
   [Enhance showRewardedAd:g_delegate placement:placement];
}

bool Defold_Enhance_isOfferwallReady(const char *pszPlacement) {
   NSString *placement = [NSString stringWithUTF8String:pszPlacement];
   return [Enhance isOfferwallReady:placement];
}

void Defold_Enhance_showOfferwall(const char *pszPlacement) {
   dmLogInfo("EnhanceDefold[iOS]: show offerwall");
   NSString *placement = [NSString stringWithUTF8String:pszPlacement];
   [Enhance showOfferwall:placement];
}

bool Defold_Enhance_isSpecialOfferReady(const char *pszPlacement) {
   NSString *placement = [NSString stringWithUTF8String:pszPlacement];
   return [Enhance isSpecialOfferReady:placement];
}

void Defold_Enhance_showSpecialOffer(const char *pszPlacement) {
   dmLogInfo("EnhanceDefold[iOS]: show special offer");
   NSString *placement = [NSString stringWithUTF8String:pszPlacement];
   [Enhance showSpecialOffer:placement];
}

bool Defold_Enhance_isFullscreenAdShowing() {
   return [[FglEnhance sharedInstance] isFullscreenAdShowing];
}

bool Defold_Enhance_isBannerAdReady(const char *pszPlacement) {
   NSString *placement = [NSString stringWithUTF8String:pszPlacement];
   return [Enhance isBannerAdReady:placement];
}

void Defold_Enhance_showBannerAd(const char *pszPlacement, const char *pszPosition) {
   Position pos = POSITION_TOP;
   
   dmLogInfo("EnhanceDefold[iOS]: show banner ad at: %s", pszPosition);
   
   if (pszPosition && !strcasecmp(pszPosition, "bottom"))
      pos = POSITION_BOTTOM;

   NSString *placement = [NSString stringWithUTF8String:pszPlacement];

   [Enhance showBannerAdWithPosition:placement position:pos];
}

void Defold_Enhance_hideBannerAd() {
   dmLogInfo("EnhanceDefold[iOS]: hide banner ad");
   [Enhance hideBannerAd];
}

void Defold_Enhance_logCustomEvent(const char *pszEventName, const char *pszParamKey, const char *pszParamValue) {
   NSString *eventName = [NSString stringWithUTF8String:pszEventName];
   
   if (pszParamKey && pszParamValue) {
      NSString *paramKey = [NSString stringWithUTF8String:pszParamKey];
      NSString *paramValue = [NSString stringWithUTF8String:pszParamValue];
      
      [Enhance logEvent:eventName withParameter:paramKey andValue:paramValue];
   }
   else {
      [Enhance logEvent:eventName];
   }
}

void Defold_Enhance_requestLocalNotificationPermission(EnhanceListener *pGranted, EnhanceListener *pRefused) {
   g_pPermissionGranted = pGranted;
   g_pPermissionRefused = pRefused;
   
   [Enhance requestLocalNotificationPermission:g_delegate];
}

void Defold_Enhance_enableLocalNotification(const char *pszTitle, const char *pszMessage, int delay) {
   NSString *title = [NSString stringWithUTF8String:pszTitle];
   NSString *msg = [NSString stringWithUTF8String:pszMessage];
   
   dmLogInfo("EnhanceDefold[iOS]: enable local notification title: %s message: %s delay:%d", pszTitle, pszMessage, delay);
   [Enhance enableLocalNotificationWithTitle:title message:msg delay:delay];
}

void Defold_Enhance_disableLocalNotification() {
   dmLogInfo("EnhanceDefold[iOS]: disable local notification");
   [Enhance disableLocalNotification];
}

void Defold_Enhance_requiresDataConsentOptIn(EnhanceListener *callback_onServiceOptInRequirement) {
    g_pOnServiceOptInRequirement = callback_onServiceOptInRequirement;
    [Enhance requiresDataConsentOptIn:g_delegate];
}

void Defold_Enhance_serviceTermsOptIn(const char *str_sdks) {
    NSString *requestedSdksString = [NSString stringWithUTF8String:str_sdks];
    
    if(![requestedSdksString isEqualToString:@""]) {
        NSArray *requestedSdks = [requestedSdksString componentsSeparatedByString:@","];
        [Enhance serviceTermsOptIn:requestedSdks];
    } else {
        [Enhance serviceTermsOptIn];
    }
}

void Defold_Enhance_showServiceOptInDialogs(const char *str_sdks, EnhanceListener *callback_onDialogComplete) {
    g_pOnDialogComplete = callback_onDialogComplete;
    NSString *requestedSdksString = [NSString stringWithUTF8String:str_sdks];
    
    if(![requestedSdksString isEqualToString:@""]) {
        NSArray *requestedSdks = [requestedSdksString componentsSeparatedByString:@","];
        [Enhance showServiceOptInDialogs:requestedSdks delegate:g_delegate];
    } else {
        NSArray *empty = [NSArray new];
        [Enhance showServiceOptInDialogs:empty delegate:g_delegate];
    }
}

void Defold_Enhance_serviceTermsOptOut() {
    [Enhance serviceTermsOptOut];
}

void Defold_Enhance_pumpEvents() {

}

bool Defold_EnhanceInAppPurchases_isSupported() {
   return [[Enhance purchases] isSupported];
}

void Defold_EnhanceInAppPurchases_attemptPurchase(const char *str_sku, EnhanceListener *callback_onPurchaseSuccess, EnhanceListener *callback_onPurchaseFailed) {
   dmLogInfo("EnhanceDefold[iOS]: attemptPurchase");
   
   NSString *sku = [NSString stringWithUTF8String:str_sku];
   
   g_pPurchaseSuccess = callback_onPurchaseSuccess;
   g_pPurchaseFailed = callback_onPurchaseFailed;
   [[Enhance purchases] attemptPurchase:sku delegate:g_delegate];
}

void Defold_EnhanceInAppPurchases_consume(const char *str_sku, EnhanceListener *callback_onConsumeSuccess, EnhanceListener *callback_onConsumeFailed) {
   dmLogInfo("EnhanceDefold[iOS]: consume");

   NSString *sku = [NSString stringWithUTF8String:str_sku];

   g_pConsumeSuccess = callback_onConsumeSuccess;
   g_pConsumeFailed = callback_onConsumeFailed;

   [[Enhance purchases] consume:sku delegate:g_delegate];
}

const char*  Defold_EnhanceInAppPurchases_getDisplayPrice(const char *str_sku, const char *str_default_price) {
   NSString *sku = [NSString stringWithUTF8String:str_sku];
   NSString *default_price = [NSString stringWithUTF8String:str_default_price];
   NSString *result = [[Enhance purchases] getDisplayPrice:sku defaultPrice:default_price];
   
   return [result UTF8String];
}

bool Defold_EnhanceInAppPurchases_isItemOwned(const char *str_sku) {
   NSString *sku = [NSString stringWithUTF8String:str_sku];
   return [[Enhance purchases] isItemOwned:sku];
}

int Defold_EnhanceInAppPurchases_getOwnedItemCount(const char *str_sku) {
   NSString *sku = [NSString stringWithUTF8String:str_sku];
   return [[Enhance purchases] getOwnedItemCount:sku];
}

void Defold_EnhanceInAppPurchases_manuallyRestorePurchases(EnhanceListener *callback_onRestoreSuccess, EnhanceListener *callback_onRestoreFailed) {
   g_pRestoreSuccess = callback_onRestoreSuccess;
   g_pRestoreFailed = callback_onRestoreFailed;

   [[Enhance purchases] manuallyRestorePurchases:g_delegate];
}

const char* Defold_EnhanceInAppPurchases_getDisplayTitle(const char *str_sku, const char *str_default_title) {
   NSString *sku = [NSString stringWithUTF8String:str_sku];
   NSString *default_title = [NSString stringWithUTF8String:str_default_title];
   NSString *result = [[Enhance purchases] getDisplayTitle:sku defaultTitle:default_title];

   return [result UTF8String];
}

const char* Defold_EnhanceInAppPurchases_getDisplayDescription(const char *str_sku, const char *str_default_description) {
   NSString *sku = [NSString stringWithUTF8String:str_sku];
   NSString *default_desc = [NSString stringWithUTF8String:str_default_description];
   NSString *result = [[Enhance purchases] getDisplayDescription:sku defaultDescription:default_desc];

   return [result UTF8String];
}

#endif
