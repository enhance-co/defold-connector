#import <Foundation/Foundation.h>

@interface FglEnhancePlus : NSObject {
}

+(id)sharedInstance;
-(void)logMessage:(NSString*)tag message:(NSString*)message;
-(void)setSdkValue:(NSString*)sdkId key:(NSString*)key value:(NSString*)value;
-(void)setCurrentScreen:(NSString*)name;

@end
