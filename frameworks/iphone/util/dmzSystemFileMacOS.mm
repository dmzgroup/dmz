#import "dmzSystemFileMacOS.h"


dmz::String
dmz_get_main_bundle_directory () {
   
   NSBundle *mainBundle = [NSBundle mainBundle];
   NSString *resourcePath = [mainBundle resourcePath];

   dmz::String result ([resourcePath UTF8String]);
   
   return result;
}


dmz::String
dmz_get_document_directory () {
   
   NSArray *paths = NSSearchPathForDirectoriesInDomains (
      NSDocumentDirectory, NSUserDomainMask, YES);
   
   NSString *documentsDirectory = [paths objectAtIndex:0];

   dmz::String result ([documentsDirectory UTF8String]);
      
   return result;
}

