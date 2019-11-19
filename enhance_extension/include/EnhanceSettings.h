//
//  EnhanceSettings.h
//  connector
//
//  Copyright © 2017 Enhance Inc. All rights reserved.
//

#ifndef EnhanceSettings_h
#define EnhanceSettings_h
#import <Foundation/Foundation.h>

@interface EnhanceSettings : NSObject {
}

+(id)sharedInstance;
-(void)setSdkConfiguration:(NSString*)sdkId configurationKey:(NSString*)configurationKey configurationValue:(NSString*)configurationValue;

@end

#endif /* EnhanceInAppPurchases_h */
