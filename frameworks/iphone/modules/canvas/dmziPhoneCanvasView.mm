#import "dmziPhoneCanvasView.h"


//@interface dmziPhoneCanvasView ()
//
//@end

@implementation dmziPhoneCanvasView


- (id)init {
     
   if (self = [super init]) {
      
      self.autoresizingMask =
         UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
   }
   
   return self;
}


- (void)dealloc {

	[super dealloc];
}


@end
