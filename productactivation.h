#ifndef PRODUCTACTIVATION_H
#define PRODUCTACTIVATION_H

#include <QFlags>
#include<QHash>

class ProductActivation
{
public:
    ProductActivation();
    virtual ~ProductActivation();

    enum class AvailableModule
    {
        NOTHING=0x0,
        PCA=0x1,
        PCB=0x2,
        PCR=0x4
    };

    Q_DECLARE_FLAGS(AvailableModules,AvailableModule)

    AvailableModules checkLicenceKey(const QString &cryptedKey);

private:
    QHash<QString,AvailableModules> cryptedKeys;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(ProductActivation::AvailableModules)

#endif // PRODUCTACTIVATION_H
