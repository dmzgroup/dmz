#import "dmzAppDelegate.h"
#import "dmzApplicationiPhone.h"
#import "dmzLogObserverNSLog.h"
#import <dmzRuntimeSession.h>
#import <dmzRuntimeConfigToTypesBase.h>
#import <dmzRuntimeConfigWrite.h>
#import <dmzSystemFile.h>
#import "dmzSystemFileMacOS.h"
#import <dmzXMLUtil.h>


@interface dmzAppDelegate (Private)

- (void)_init;
- (void)_update_time_slice;

@end


@implementation dmzAppDelegate

static dmzAppDelegate *localSharedApp = NULL;


@synthesize window;
@synthesize rootController;


- (id)init {
   
	if (!localSharedApp) {
      
		localSharedApp = [super init];
	}
   
	return localSharedApp;
}


// Initialize the singleton instance if needed and return
+ (dmzAppDelegate *)shared_dmz_app
{
	if (!localSharedApp) {
      
		localSharedApp = [[dmzAppDelegate alloc] init];
   }
	
	return localSharedApp;
}


- (void)dealloc {
   
NSLog (@"dmzAppDelegate: dealloc");
   
   if (_obs) { delete _obs; _obs = 0; }
   if (_app) { delete _app; _app = 0; }
   
   [rootController release];
	[window release];
   
	[super dealloc];
}


- (void)applicationDidFinishLaunching:(UIApplication *)application {	
   
   [self _init];
   
   if (_frequency == 0.0) { _frequency = 30.0; }
   
   _timer = [NSTimer scheduledTimerWithTimeInterval:(1.0 / _frequency)
      target:self selector:@selector(_update_time_slice) userInfo:nil repeats:YES];   
   
   [window addSubview:[rootController view]];
   [window makeKeyAndVisible];
}


- (void) applicationWillTerminate: (UIApplication *) application {
   
   [_timer invalidate];
   _timer = nil;
   
   if (_app) {
      
      _app->stop ();
      _app->unload_plugins ();
      _app->save_session ();
   }
}


- (dmz::RuntimeContext *)get_runtime_context {

   dmz::RuntimeContext *context (0);
   
   if (_app) { context = _app->get_context (); }
   
   return context;
}


- (void)_init {

   _app = new dmz::ApplicationiPhone ("dmz", "dmz");
   _obs = new dmz::LogObserverNSLog (_app->get_context ());
   
   _app->load_session ();

   dmz::String workingDir (dmz_get_main_bundle_directory ());
   
   dmz::String manifestFile (dmz::format_path (workingDir + "/manifest.xml"));
   
   dmz::PathContainer fileList;
   
   dmz::Config global ("global");
   
   _app->log.info << "Manifest file: " << manifestFile << dmz::endl;
   
   if (dmz::is_valid_path (manifestFile)) {
      
      if (dmz::xml_to_config (manifestFile, global, &(_app->log))) {
         
         dmz::Config manifest;
         
         if (global.lookup_all_config_merged ("manifest", manifest)) {
            
            _frequency = dmz::config_to_float64 (
               "frequency.value", manifest, 30.0);
            
            _app->log.warn << "Frequency: " << _frequency << dmz::endl;
            
            dmz::ConfigIterator it;
            dmz::Config cd;
            dmz::String value;
            
            while (manifest.get_next_config (it, cd)) {
               
               const dmz::String DataName (cd.get_name ().to_lower ());
               
               if (DataName == "config") {
                  
                  if (cd.lookup_attribute ("file", value)) {
                     
                     fileList.add_path (dmz::format_path (workingDir + "/" + value));
                  }
               }
            }
            
            dmz::String versionFile (dmz::format_path (workingDir + "/version.xml"));
            
            if (dmz::is_valid_path (versionFile)) {
               
               fileList.add_path (versionFile);
            }
         }
      }
   }
   
   _app->load_config (fileList);
   _app->load_plugins ();
   _app->start ();
}


- (void)_update_time_slice {
   
   if (_app) {
      
      if (!_app->update_time_slice ()) {
         
         [_timer invalidate];
         _timer = nil;
         
//         _app->stop ();
//         _app->unload_plugins ();
//         _app->save_session ();
      }
   }
}


@end
