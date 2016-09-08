#include "productactivation.h"

#include <QString>


ProductActivation::ProductActivation()
{
    cryptedKeys.insert("1111",AvailableModule::PCA);
    cryptedKeys.insert("2222",AvailableModule::PCB);
    cryptedKeys.insert("3333",AvailableModule::PCR);
    cryptedKeys.insert("4444",AvailableModule::PCA|AvailableModule::PCB);
    cryptedKeys.insert("5555",AvailableModule::PCA|AvailableModule::PCR);
    cryptedKeys.insert("6666",AvailableModule::PCR|AvailableModule::PCB);
    cryptedKeys.insert("7777",AvailableModule::PCA|AvailableModule::PCB|AvailableModule::PCR);
}

ProductActivation::~ProductActivation()
{
    cryptedKeys.clear();
}



ProductActivation::AvailableModules ProductActivation::checkLicenceKey(const QString &cryptedKey)
{
    if(cryptedKeys.contains(cryptedKey))
    {
        return cryptedKeys.value(cryptedKey);
    }
    else
    {
        return AvailableModule::NOTHING;
    }

}
