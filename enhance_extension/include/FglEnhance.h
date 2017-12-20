#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import "Enhance.h"

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
