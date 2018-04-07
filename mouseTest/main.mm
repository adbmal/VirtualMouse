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
        OSXScreen screen;
        screen.updateScreenShape();
        screen.hideCursor();
        [[NSRunLoop currentRunLoop] run];
    }
    return 0;
}

