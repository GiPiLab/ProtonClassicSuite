#ifndef PRODUCTACTIVATION_H
#define PRODUCTACTIVATION_H

#include <QFlags>
#include <QHash>
#include <QSettings>

class ProductActivation
{
public:
    ProductActivation();
    virtual ~ProductActivation();

    enum class AvailableModule
    {
        NOTHING=0,
        PCA=0x1,
        PCB=0x2,
        PCR=0x4,
        DEMO=0x8
    };

    Q_DECLARE_FLAGS(AvailableModules,AvailableModule)




    const QString availableModulesToString(ProductActivation::AvailableModules modules) const;

    bool askForActivationKey();

    const ProductActivation::AvailableModules getAvailablesModules() const{return availableModules;}




private:
    const QByteArray hashActivationKeyToHex(const QString &activationKey) const;

    const AvailableModules computeAvailablesModules(const QByteArray &hashedHexKey) const;
    QHash<QByteArray,AvailableModules> hashedKeys;
    ProductActivation::AvailableModules availableModules;



};

Q_DECLARE_OPERATORS_FOR_FLAGS(ProductActivation::AvailableModules)

#endif // PRODUCTACTIVATION_H
