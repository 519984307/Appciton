#pragma once
#include "Provider.h"

class LoFloProvider: public Provider
{
public:
    virtual bool attachParam(Param &param);
    virtual void dataArrived(void);

    LoFloProvider();
    virtual ~LoFloProvider();
};
