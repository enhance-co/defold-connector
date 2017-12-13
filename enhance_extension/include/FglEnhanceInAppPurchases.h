#import <Foundation/Foundation.h>
#import "EnhanceInAppPurchases.h"

@interface FglEnhanceInAppPurchases : NSObject {
}

+(id)sharedInstance;
-(BOOL)isSupported;
-(void)attemptPurchase:(NSString*)sku delegate:(id)delegate;
-(NSString*)getDisplayPrice:(NSString*)sku defaultPrice:(NSString*)defaultPrice;
-(BOOL)isItemOwned:(NSString*)sku;
-(void)restorePurchases:(id)delegate;

@end
