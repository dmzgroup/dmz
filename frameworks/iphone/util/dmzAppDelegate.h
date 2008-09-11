#import <UIKit/UIKit.h>
#import <dmzTypes.h>

namespace dmz {
   
   class ApplicationiPhone;
   class LogObserverNSLog;
   class RuntimeContext;
};


@interface dmzAppDelegate : NSObject <UIApplicationDelegate> {
   
	IBOutlet UIWindow *window;
   UIViewController *rootController;
   
   dmz::ApplicationiPhone *_app;
   dmz::LogObserverNSLog *_obs;
   dmz::Float64 _frequency;
   NSTimer *_timer;
}


@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) UIViewController *rootController;

+ (dmzAppDelegate *)shared_dmz_app;

- (dmz::RuntimeContext *)get_runtime_context;


@end

