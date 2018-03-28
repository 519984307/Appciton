#include "LoFloProvider.h"

bool LoFloProvider::attachParam(Param &param)
{
    param.getName();
    return true;
}

void LoFloProvider::dataArrived(void)
{

}

LoFloProvider::LoFloProvider() : Provider("LoFlo")
{
    PortAttrDesc port_attr(9600, 8, 'N', 1);
    initPort(port_attr);
}

LoFloProvider::~LoFloProvider()
{

}

