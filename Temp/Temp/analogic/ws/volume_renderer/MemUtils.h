//------------------------------------------------------------------------------
// File: MemUtils.h.
// Description: Check the memory availabnle to the application
// Copyright 2016 Analogic Corp.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef ANALOGIC_WS_VOLUME_RENDERER_MEMUTILS_H_
#define ANALOGIC_WS_VOLUME_RENDERER_MEMUTILS_H_
//------------------------------------------------------------------------------

#include <QDebug>
#include <stdint.h>


namespace MemUtils
{

bool testForAvailableMemory(uint64_t bytesRequsted,
                            uint64_t& bytesAvailable);

void getTotalAndFreeMemoryBytes(uint64_t& bytesTotal,
                            uint64_t& bytesAvailable);


//------------------------------------------------------------------------------
}  // namespace MemUtils
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#endif  // ANALOGIC_WS_VOLUME_RENDERER_MEMUTILS_H_
//------------------------------------------------------------------------------
