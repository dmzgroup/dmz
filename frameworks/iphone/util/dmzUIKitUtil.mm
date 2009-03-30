#import "dmzUIKitUtil.h"

void
UIKitImproveSliderAccuracy (UISlider *slider) {
   
   CGRect frame = slider.frame;
   slider.frame = CGRectMake (frame.origin.x, frame.origin.y, frame.size.width, 35.0);
   
   UIImage* thumbImage = [UIImage imageNamed:@"slider_thumb.png"];
   [slider setThumbImage:thumbImage forState:UIControlStateNormal];
   
   UIImage* leftImage = [UIImage imageNamed:@"slider_left.png"];
   [leftImage stretchableImageWithLeftCapWidth:5 topCapHeight:0];
   [slider setMinimumTrackImage:leftImage forState:UIControlStateNormal];
   
   UIImage* rightImage = [UIImage imageNamed:@"slider_right.png"];
   [rightImage stretchableImageWithLeftCapWidth:5.0 topCapHeight:9.0];
   [slider setMaximumTrackImage:rightImage forState:UIControlStateNormal];
}
