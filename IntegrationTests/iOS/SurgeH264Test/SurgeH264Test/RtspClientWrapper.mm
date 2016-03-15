//
//  RtspClientWrapper.mm
//  SurgeH264Test
//
//  Created by Philip Herron on 07/03/2016.
//  Copyright © 2016 Philip Herron. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RtspClientWrapper.h"

#include "Surge.h"

class SurgeDelegate: public Surge::RtspClientDelegate {
public:
    
    SurgeDelegate(id<RtspClientDelegate> delegate): m_delegate(delegate) {}
    
    void ClientDidTimeout() {
        [m_delegate Timeout];
    }
    
    void Payload(const char* buffer, size_t length) {
        [m_delegate Payload:(const unsigned char *)buffer withSize:length];
    }
    
private:
    id<RtspClientDelegate> m_delegate;
};

@interface RtspClientWrapper ()

@property (nonatomic, strong) NSString* url;
@property (nonatomic, assign) Surge::RtspClientDelegate *rtspDelegate;
@property (nonatomic, assign) Surge::RtspClient *client;

@end

@implementation RtspClientWrapper

- (id)initWithUrl:(NSString*)url andDelegate:(id<RtspClientDelegate>)delegate {
    if (self = [super init]) {
        self.url = url;
        self.rtspDelegate = new SurgeDelegate(delegate);
        self.client = new Surge::RtspClient(self.rtspDelegate);
    }
    return self;
}

- (void)dealloc {
    delete self.client;
    delete self.rtspDelegate;
}

- (void)playStream {
    Surge::DescribeResponse *describe_resp = self.client->Describe(std::string([self.url UTF8String]),
                                                                   false, "", "");
    Surge::SessionDescription palette = describe_resp->GetPalettes()[0];
    delete describe_resp;
    
    Surge::RtspResponse *setup_resp = self.client->Setup(palette);
    delete setup_resp;
    
    Surge::RtspResponse *play_resp = self.client->Play(false);
    delete play_resp;
}

@end
