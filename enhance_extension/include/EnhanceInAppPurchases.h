//
//  EnhanceInAppPurchases.h
//  connector
//
//  Copyright © 2017 Enhance Inc. All rights reserved.
//

#ifndef EnhanceInAppPurchases_h
#define EnhanceInAppPurchases_h
#import <Foundation/Foundation.h>

@protocol PurchaseDelegate<NSObject>

-(void)onPurchaseSuccess;
-(void)onPurchaseFailed;

@end

@protocol RestoreDelegate<NSObject>

-(void)onRestoreSuccess;
-(void)onRestoreFailed;

@end

@protocol ConsumeDelegate<NSObject>

-(void)onConsumeSuccess;
-(void)onConsumeFailed;

@end

@interface EnhanceInAppPurchases : NSObject {
}

+(id)sharedInstance;
-(BOOL)isSupported;
-(void)attemptPurchase:(NSString*)sku delegate:(id)delegate;
-(NSString*)getDisplayPrice:(NSString*)sku defaultPrice:(NSString*)defaultPrice;
-(NSString*)getDisplayTitle:(NSString*)sku defaultTitle:(NSString*)defaultTitle;
-(NSString*)getDisplayDescription:(NSString*)sku defaultDescription:(NSString*)defaultDescription;
-(BOOL)isItemOwned:(NSString*)sku;
-(int)getOwnedItemCount:(NSString*)sku;
-(void)consume:(NSString*)sku delegate:(id)delegate;
-(void)manuallyRestorePurchases:(id)delegate;

@end

#endif /* EnhanceInAppPurchases_h */
