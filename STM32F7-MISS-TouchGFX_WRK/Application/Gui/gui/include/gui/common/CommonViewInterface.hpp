#ifndef COMMON_VIEW_INTERFACE_HPP
#define COMMON_VIEW_INTERFACE_HPP

#include <touchgfx/hal/Types.hpp>

using namespace touchgfx;

class CommonViewInterface
{
public:
    virtual ~CommonViewInterface() {}
    virtual void updateProcessorLoad(uint8_t mcuLoadInProcent) {}
};

#endif
