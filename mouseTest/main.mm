//
//  main.m
//  mouseTest
//
//  Created by Yiwei Wang on 06/04/2018.
//  Copyright © 2018 Yiwei Wang. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "mouse.hpp"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
//            OSXScreen screen;
//            screen.updateScreenShape();
//            screen.hideCursor();
//            [[NSRunLoop currentRunLoop] run];
        });
//        int age;
//        scanf("%d", &age);
        [[NSRunLoop currentRunLoop] run];
    }
    return 0;
}


