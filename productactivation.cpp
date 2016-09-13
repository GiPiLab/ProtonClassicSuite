#include "productactivation.h"
#include <QCryptographicHash>

#include <QString>
#include <QInputDialog>



ProductActivation::ProductActivation()
{
    hashedKeys.insert("7357d2d1fce5c95f2e19f6779da72cb8d9afc174efb6c0d0bffdede9c2da37fd8360772268e26bbcb22ececc8bb45666",AvailableModule::PCA);
    hashedKeys.insert("16f37125c91a66bcc499f918696cf271e7f7d3c7e3fc794b67dbf684002abd1d46c9388cef507c4d00031d0c3d6410b4",AvailableModule::PCB);
    hashedKeys.insert("43b8bd4ddeee40150945d1a0cfa6f086afd21cc035d952659c62f5d6ed4d72df496b739485f30bc983492b4062c4186c",AvailableModule::PCR);
    hashedKeys.insert("0c8013ab87c231566dfcd2f0126ba8ca1e1256b4f17f50f8dc750d91ee61d337552c5264937782c1c8c0fb803125d4dd",AvailableModule::PCA|AvailableModule::PCB);
    hashedKeys.insert("531d7a83142a0274c7048584d4f1e4fee39b72bb5737e72f7a268b93fa938f153884aff5908073e5508adb7d40e158dc",AvailableModule::PCA|AvailableModule::PCR);
    hashedKeys.insert("8c295eeb24eb3a932c4245b56cecafb2c0e6572e52a0b3d74f65cdabafcc9f5d567528ac9c4feac7c6431172f2670667",AvailableModule::PCR|AvailableModule::PCB);
    hashedKeys.insert("43564d8fa2bc70882bc5f1df5ebce3e649ebc2cb2bb7d424b56aa9b5339c281d74d33c4cdf1536033425bd6d4c66287b",AvailableModule::PCA|AvailableModule::PCB|AvailableModule::PCR);

    QSettings settings;
    QByteArray productKey=settings.value("licence/productkey").toByteArray();
    availableModules=computeAvailablesModules(productKey);

}

ProductActivation::~ProductActivation()
{
    hashedKeys.clear();
}


const ProductActivation::AvailableModules ProductActivation::computeAvailablesModules(const QByteArray &hashedHexKey) const
{
    if(hashedKeys.contains(hashedHexKey))
    {
        return hashedKeys.value(hashedHexKey);
    }
    else
    {
        return AvailableModule::NOTHING;
    }
}

const QByteArray ProductActivation::hashActivationKeyToHex(const QString &activationKey) const
{
    return QCryptographicHash::hash(activationKey.toLatin1(),QCryptographicHash::Sha384).toHex();
}

const QString ProductActivation::availableModulesToString(ProductActivation::AvailableModules modules) const
{
    QStringList activatedModules;


    if(modules & AvailableModule::PCA)
        activatedModules.append(QObject::tr("Auditeur"));
    if(modules & AvailableModule::PCB)
        activatedModules.append(QObject::tr("Budget"));
    if(modules & AvailableModule::PCR)
        activatedModules.append(QObject::tr("Reporting"));

    if(!activatedModules.isEmpty())
    {
        return activatedModules.join('+');
    }
    return QObject::tr("Non activé");

}

bool ProductActivation::askForActivationKey()
{
    bool ok;
    QString text;

    do
    {
        text=QInputDialog::getText(nullptr,QObject::tr("Activation"), QObject::tr("Entrez votre clé d'activation :"),QLineEdit::Normal,"",&ok);

    }while(ok && text.isEmpty());

    if(ok)
    {
        QByteArray hashedKey=hashActivationKeyToHex(text);
        availableModules=computeAvailablesModules(hashedKey);

        if(availableModules&ProductActivation::AvailableModule::PCA || availableModules&ProductActivation::AvailableModule::PCR
                ||availableModules&ProductActivation::AvailableModule::PCB)
        {
            QSettings settings;
            settings.setValue("licence/productkey",hashedKey);
            return true;

        }
    }
    return false;
}
