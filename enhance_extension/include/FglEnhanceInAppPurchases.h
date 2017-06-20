#import <Foundation/Foundation.h>

@protocol PurchaseDelegate<NSObject>

-(void)onPurchaseSuccess;
-(void)onPurchaseFailed;

@end

@protocol RestoreDelegate<NSObject>

-(void)onRestoreSuccess;
-(void)onRestoreFailed;

@end

@interface FglEnhanceInAppPurchases : NSObject {
}

+(id)sharedInstance;
-(BOOL)isSupported;
-(void)attemptPurchase:(NSString*)sku delegate:(id)delegate;
-(NSString*)getDisplayPrice:(NSString*)sku defaultPrice:(NSString*)defaultPrice;
-(BOOL)isItemOwned:(NSString*)sku;
-(void)restorePurchases:(id)delegate;

@end
