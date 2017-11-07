//
//  SURFaceDetectionImageView.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 02/10/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import "SURFaceDetectionImageView.h"

#define FACE_DETECTION_DEBUG_MODE 1

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

- (void)detectFaces {
  if (!self.image) {
    [self queueNextFaceDetection];
    return;
  }
  if (FACE_DETECTION_DEBUG_MODE) NSLog(@"Detecting faces in image...");
  CIImage *image = [CIImage imageWithCGImage:self.image.CGImage];
  CGSize imageSize = image.extent.size;
  CGAffineTransform transform = CGAffineTransformMakeScale(1, -1);
  transform = CGAffineTransformTranslate(transform, 0, -imageSize.height);
  
  NSDictionary *opts = @{CIDetectorImageOrientation : @(kCGImagePropertyOrientationUp) };
  NSArray *features = [self.faceDetector featuresInImage:image options:opts];
  if (FACE_DETECTION_DEBUG_MODE) NSLog(@"found %@ features...", @(features.count));
  NSMutableArray<NSValue *> *faceRects = [NSMutableArray new];
  for (CIFaceFeature *feature in features) {
    CGRect faceViewBounds = CGRectApplyAffineTransform(feature.bounds, transform);
    
    // Calculate the actual position and size of the rectangle in the image view
    CGSize viewSize = self.bounds.size;
    CGFloat scale = MIN(viewSize.width / imageSize.width,
                        viewSize.height / imageSize.height);
    CGFloat offsetX = (viewSize.width - imageSize.width * scale) / 2;
    CGFloat offsetY = (viewSize.height - imageSize.height * scale) / 2;
    
    faceViewBounds = CGRectApplyAffineTransform(faceViewBounds, CGAffineTransformMakeScale(scale, scale));
    faceViewBounds.origin.x += offsetX;
    faceViewBounds.origin.y += offsetY;
    [faceRects addObject:[NSValue valueWithCGRect:faceViewBounds]];
  }
  self.overlayView.faceRects = faceRects;
  [self queueNextFaceDetection];
}

- (void)queueNextFaceDetection {
  if (self.faceDetectionEnabled) {
    [self performSelector:@selector(detectFaces) withObject:nil afterDelay:0.1];
  }
}

- (void)stopFaceDetection {
  [NSObject cancelPreviousPerformRequestsWithTarget:self];
  [self.overlayView clear];
}

- (void)startFaceDetection {
  if (FACE_DETECTION_DEBUG_MODE) NSLog(@"Starting face detection...");
  [self detectFaces];
}

- (void)setFaceDetectionEnabled:(BOOL)faceDetectionEnabled {
  _faceDetectionEnabled = faceDetectionEnabled;
  if (faceDetectionEnabled) {
    [self startFaceDetection];
  }
  else {
    [self stopFaceDetection];
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
