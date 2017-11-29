//
//  SURFaceDetectionImageView.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 02/10/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import "SURFaceDetectionImageView.h"

@interface FaceRectsOverlayView : UIView
@property (nonatomic, copy) NSArray<NSValue *> *faceRects;
- (void)clear;
@end

@interface SURFaceDetectionImageView ()
@property (nonatomic, strong) CIDetector *faceDetector;
@property (weak, nonatomic) IBOutlet UIButton *faceDetectionToggleButton;
@property (strong, nonatomic) FaceRectsOverlayView *overlayView;
@end

@implementation SURFaceDetectionImageView

- (instancetype)initWithFrame:(CGRect)frame {
  self = [super initWithFrame:frame];
  if (self) {
    [self initialiseFaceDetection];
  }
  return self;
}

- (void)awakeFromNib {
  [super awakeFromNib];
  [self initialiseFaceDetection];
}

- (void)dealloc {
  self.faceDetectionEnabled = NO;
}

#pragma mark - Faces

- (void)initialiseFaceDetection {
  self.overlayView = [[FaceRectsOverlayView alloc] initWithFrame:self.bounds];
  self.overlayView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
  [self addSubview:self.overlayView];
  
  CIContext *context = [CIContext context];
  NSDictionary *opts = @{ CIDetectorAccuracy : CIDetectorAccuracyHigh };
  self.faceDetector = [CIDetector detectorOfType:CIDetectorTypeFace
                                         context:context
                                         options:opts];
  self.faceDetectionEnabled = NO;
}

- (void)detectFacesIn:(UIImage *)imageIn forViewSize:(CGSize)viewSize {
  if (!imageIn) {
    [self enqueueNextFaceDetection];
    return;
  }
  CIImage *image = [CIImage imageWithCGImage:imageIn.CGImage];
  CGSize imageSize = image.extent.size;
  CGAffineTransform transform = CGAffineTransformMakeScale(1, -1);
  transform = CGAffineTransformTranslate(transform, 0, -imageSize.height);
  
  NSDictionary *opts = @{CIDetectorImageOrientation : @(kCGImagePropertyOrientationUp) };
  NSArray *features = [self.faceDetector featuresInImage:image options:opts];
  NSMutableArray<NSValue *> *faceRects = [NSMutableArray new];
  for (CIFaceFeature *feature in features) {
    CGRect faceViewBounds = CGRectApplyAffineTransform(feature.bounds, transform);
    
    // Calculate the actual position and size of the rectangle in the image view
    CGFloat scale = MIN(viewSize.width / imageSize.width,
                        viewSize.height / imageSize.height);
    CGFloat offsetX = (viewSize.width - imageSize.width * scale) / 2;
    CGFloat offsetY = (viewSize.height - imageSize.height * scale) / 2;
    
    faceViewBounds = CGRectApplyAffineTransform(faceViewBounds, CGAffineTransformMakeScale(scale, scale));
    faceViewBounds.origin.x += offsetX;
    faceViewBounds.origin.y += offsetY;
    [faceRects addObject:[NSValue valueWithCGRect:faceViewBounds]];
  }
  dispatch_async(dispatch_get_main_queue(), ^{
    self.overlayView.faceRects = faceRects;
    [self enqueueNextFaceDetection];
  });
}

- (void)enqueueNextFaceDetection {
  if (self.faceDetectionEnabled) {
    UIImage *image = self.image;
    CGSize viewSize = self.bounds.size;
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0);
    NSTimeInterval dt = 0.1;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(dt * NSEC_PER_SEC)), queue, ^{
      if (self) {
        [self detectFacesIn:image forViewSize:viewSize];
      }
    });
  }
}

- (void)startFaceDetection {
  [self enqueueNextFaceDetection];
}

- (void)setFaceDetectionEnabled:(BOOL)faceDetectionEnabled {
  _faceDetectionEnabled = faceDetectionEnabled;
  if (faceDetectionEnabled) {
    [self startFaceDetection];
  }
  else {
    [self.overlayView performSelector:@selector(clear) withObject:nil afterDelay:0.2];
  }
  NSString *imageName = [NSString stringWithFormat:@"faces-%@", faceDetectionEnabled ? @"on" : @"off"];
  UIImage *image = [[UIImage imageNamed:imageName] imageWithRenderingMode:UIImageRenderingModeAlwaysTemplate];
  [self.faceDetectionToggleButton setImage:image forState:UIControlStateNormal];
}

#pragma mark - Actions

- (IBAction)faceDetectionToggleButtonAction:(UIButton *)sender {
  self.faceDetectionEnabled = !self.faceDetectionEnabled;
}

@end

@implementation FaceRectsOverlayView

- (instancetype)initWithFrame:(CGRect)frame {
  self = [super initWithFrame:frame];
  if (self) {
    _faceRects = [NSArray<NSValue *> new];
    self.backgroundColor = [UIColor clearColor];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(orientationDidChange:)
                                                 name:UIDeviceOrientationDidChangeNotification
                                               object:nil];
  }
  return self;
}

- (void)dealloc {
  [[NSNotificationCenter defaultCenter] removeObserver:self name:UIDeviceOrientationDidChangeNotification object:nil];
}

#pragma mark - Rotations

- (void)orientationDidChange:(NSNotification *)notification {
  [self clear];
}

#pragma mark - Drawing

- (void)clear {
  _faceRects = [NSArray<NSValue *> new];
  [self setNeedsDisplay];
}

- (void)setFaceRects:(NSArray<NSValue *> *)faceRects {
  [self clear];
  _faceRects = faceRects;
  [self setNeedsDisplay];
}

- (void)drawRect:(CGRect)rect {
  [super drawRect:rect];
  UIColor *boxColor = [UIColor redColor];
  [boxColor set];
  
  for (NSValue *val in self.faceRects) {
    UIBezierPath *path = [UIBezierPath bezierPathWithRect:val.CGRectValue];
    [path stroke];
  }
}

@end
