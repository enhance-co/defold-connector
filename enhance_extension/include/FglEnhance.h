#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

#define PLACEMENT_DEFAULT @"default"

#define INTERSTITIAL_PLACEMENT_DEFAULT @"default"
#define INTERSTITIAL_PLACEMENT_LEVEL_COMPLETED @"level_completed"

#define REWARDED_PLACEMENT_SUCCESS @"SUCCESS"
#define REWARDED_PLACEMENT_HELPER @"HELPER"
#define REWARDED_PLACEMENT_NEUTRAL @"NEUTRAL"

typedef enum {
   REWARDTYPE_ITEM = 1,
   REWARDTYPE_COINS,
} RewardType;

typedef enum {
   POSITION_TOP = 1,
   POSITION_BOTTOM,
} Position;

@protocol InterstitialDelegate<NSObject>

-(void)onInterstitialCompleted;

@optional
-(void)onInterstitialShowing;

@end

@protocol CurrencyGrantedDelegate<NSObject>

-(void)onCurrencyGranted:(int)amount;

@end

@protocol RewardDelegate<NSObject>

-(void)onRewardGranted:(int)rewardValue rewardType:(RewardType)rewardType;
-(void)onRewardDeclined;
-(void)onRewardUnavailable;

@optional
-(void)onRewardedAdShowing;

@end

@protocol PermissionDelegate<NSObject>

-(void)onPermissionGranted;
-(void)onPermissionRefused;

@end

@protocol AppConfigDelegate<NSObject>

-(void)onAppConfigReceived:(NSDictionary*)config;

@end

@interface FglEnhance : NSObject {
}

+(id)sharedInstance;
-(void)setInterstitialDelegate:(id)delegate;
-(void)setCurrencyGrantedDelegate:(id)delegate;
-(void)setAppConfigDelegate:(id)delegate;
-(BOOL)isEnhanced;
-(BOOL)isInterstitialReady;
-(BOOL)isInterstitialReady:(NSString*)placement;
-(void)showInterstitial;
-(void)showInterstitial:(NSString*)placement;
-(BOOL)isRewardedAdReady;
-(BOOL)isRewardedAdReady:(NSString*)placement;
-(void)showRewardedAd:(id)delegate;
-(void)showRewardedAd:(id)delegate placement:(NSString*)placement;
-(BOOL)isFullscreenAdShowing;
-(BOOL)isOverlayAdReady;
-(void)showOverlayAd:(Position)position;
-(void)showOverlayAdWithRect:(CGRect)rect;
-(void)hideOverlayAd;
-(BOOL)isBannerAdReady;
-(BOOL)isBannerAdReady:(NSString*)placement;
-(void)showBannerAd:(Position)position;
-(void)showBannerAd:(NSString*)placement position:(Position)position;
-(void)showBannerAdWithRect:(CGRect)rect;
-(void)showBannerAdWithRect:(NSString*)placement rect:(CGRect)rect;
-(void)hideBannerAd;
-(BOOL)isSpecialOfferReady;
-(BOOL)isSpecialOfferReady:(NSString*)placement;
-(void)showSpecialOffer;
-(void)showSpecialOffer:(NSString*)placement;
-(BOOL)isOfferwallReady;
-(BOOL)isOfferwallReady:(NSString*)placement;
-(void)showOfferwall;
-(void)showOfferwall:(NSString*)placement;
-(void)logEvent:(NSString*)eventType;
-(void)logEvent:(NSString*)eventType withParameter:(NSString*)paramKey andValue:(NSString*)paramValue;
-(void)requestLocalNotificationPermission:(id)delegate;
-(void)enableLocalNotificationWithTitle:(NSString*)title message:(NSString*)message delay:(int)delay;
-(void)disableLocalNotification;

@end
