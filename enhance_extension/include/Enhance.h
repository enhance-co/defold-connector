//
//  Enhance.h
//  connector
//
//  Copyright © 2017 Enhance Inc. All rights reserved.
//

#ifndef Enhance_h
#define Enhance_h

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import "EnhanceInAppPurchases.h"

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

@interface Enhance : NSObject {
}

+(BOOL)isInterstitialReady;
+(BOOL)isInterstitialReady:(NSString*)placement;
+(void)showInterstitialAd;
+(void)showInterstitialAd:(NSString*)placement;
+(BOOL)isRewardedAdReady;
+(BOOL)isRewardedAdReady:(NSString*)placement;
+(void)showRewardedAd:(id)delegate;
+(void)showRewardedAd:(id)delegate placement:(NSString*)placement;
+(BOOL)isBannerAdReady;
+(BOOL)isBannerAdReady:(NSString*)placement;
+(void)showBannerAdWithPosition:(Position)position;
+(void)showBannerAdWithPosition:(NSString*)placement position:(Position)position;
+(void)hideBannerAd;
+(BOOL)isSpecialOfferReady;
+(BOOL)isSpecialOfferReady:(NSString*)placement;
+(void)showSpecialOffer;
+(void)showSpecialOffer:(NSString*)placement;
+(BOOL)isOfferwallReady;
+(BOOL)isOfferwallReady:(NSString*)placement;
+(void)showOfferwall;
+(void)showOfferwall:(NSString*)placement;
+(void)setCurrencyGrantedDelegate:(id)delegate;
+(void)requestLocalNotificationPermission:(id)delegate;
+(void)enableLocalNotificationWithTitle:(NSString*)title message:(NSString*)message delay:(int)delay;
+(void)disableLocalNotification;
+(void)logEvent:(NSString*)eventType;
+(void)logEvent:(NSString*)eventType withParameter:(NSString*)paramKey andValue:(NSString*)paramValue;
+(EnhanceInAppPurchases*)purchases;

+(BOOL)isEnhanced;
+(BOOL)isFullscreenAdShowing;

@end


#endif /* Enhance_h */
