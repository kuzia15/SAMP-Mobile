//
// Created by plaka on 02.05.2024.
//

#include "p2core.h"
#include "../vendor/armhook//patch.h"

RxPipeline* RxPipelineCreate() {
    return CHook::CallFunction<RxPipeline*>("_Z16RxPipelineCreatev");
}