#include "productactivation.h"
#include <QCryptographicHash>

#include <QString>
#include <QInputDialog>



ProductActivation::ProductActivation()
{
    hashedKeys.insert("0882f27431c1ac4096ae595b146972b27b24fc4c80a1fdd21d8573828a4df35246a5e8ae9f8ab8dbe2e591cb5c17936a84f86e157371703f88aa810b67766315",AvailableModule::PCA);
    hashedKeys.insert("dd50ccea91f3101d8824e58ce203838b5376bee3ba07df02a34c2b9532c320feb126a75106ee2de046e53d6fd3b2a2d2b6ef3dc883d76b31bc575c80961acf73",AvailableModule::PCB);
    hashedKeys.insert("77d22c1babeab292189bc1f6d0e82ef8d23107656d707c153c736d01276409f610a0638bdcb107e5a3eba3650b632d6b1e4ed6b423b6c736f677db52a35af21f",AvailableModule::PCR);
    hashedKeys.insert("918633464913f4f9b5d700d157866cbcd67cc7f108006b7a6181d1ca4077009e82ec8d2508c03f79d9ebdd8005a7a87a38ede889d85f72e955ca99677c27493d",AvailableModule::PCA|AvailableModule::PCB);
    hashedKeys.insert("3cf8ddbf43ae53a03a84115a56ab98b5256fa579d94c52ac8c833d333c9ebe5f9fd25b2a9a4bc6a59314a94d4814792d4dccbebb089ebbcaf64a42dcdecbd2f5",AvailableModule::PCA|AvailableModule::PCR);
    hashedKeys.insert("04ffe56309c075f2f7447348b0ae55d0faa2e0661cde1f65df0f30cbd13b3ffc82fc46a294b838e1fc99b3d2a49d7ef0458111b17a44208e53bf61da5a87bd60",AvailableModule::PCR|AvailableModule::PCB);
    hashedKeys.insert("c755c4c9e221648b2a21998625607111c8159a7fffcd36ca20f7d9c6855a7339e3ed343bad4615c67d6ff10ac76c527fee7b12c62a63de6577b798dedc2855b2",AvailableModule::PCA|AvailableModule::PCB|AvailableModule::PCR);

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
    QByteArray level2Hash=QCryptographicHash::hash(hashedHexKey,QCryptographicHash::Sha512).toHex();
    if(hashedKeys.contains(level2Hash))
    {
        return hashedKeys.value(level2Hash);
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
