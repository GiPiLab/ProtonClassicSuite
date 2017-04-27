#include "pcx_tree.h"
#include "utils.h"
#include "xlsxdocument.h"
#include <ctime>
#include <QMessageBox>
#include <QDateTime>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QUuid>
#include <QElapsedTimer>
#include <QFileInfo>

QStringList PCx_Tree::firstNameList={"Aaron","Abdonie","Abdonise","Abel","Abélard","Abélarde","Abel-François","Abélie","Abelin","Abeline","Abella","Abelle","Abigaelle","Abigaïl","Abraham","Achille","Adalbert","Adam","Adel","Adélaïde","Adèle","Adélia","Adélie","Adeline","Adelphe","Adelphine","Adémar","Adolph","Adolphe","Adrien","Adrienne","Agathe","Aglaé","Agnès","Agrippin","Aimé","Aimée","Alain","Alban","Albane","Albéric","Albert","Alberte","Albertin","Albertine","Albin","Alcée",
                                     "Alde","Aldric","Alexandra","Alexandre","Alexandrine","Alexia","Alexis","Alfred","Alice","Alicia","Alida","Aliénor","Aline","Aliptien","Alison","Alma","Almaric","Aloïs","Aloïse","Aloïsse","Aloys","Aloysa","Aloyse","Aloysia","Alphonse","Alphonsine","Alvare","Amace","Amadou","Amadour","Amalric","Amance","Amand","Amanda","Amande","Amandine","Amandio","Amans","Amant","Amante","Amanthe","Amantine","Amaury","Ambre","Ambroise","Amédée","Amel","Amélie","Ameline","Améline","Amerigo","Anabelle","Anaëlle","Anaïs","Anastasie","Anatole","André","Andréa","Andréanne","Andrée","Andy","Ange","Angèle","Angeline","Angélique","Anicet",
                                     "Anita","Anna","Annah","Anne","Anne-Charlotte","Anne-Laure","Anneli","Anne-Marie","Anne-Sophie","Annette","Annick","Annie","Anny","Anselme","Anthelme","Anthony","Antigone","Antoine","Antoinette","Antoni","Antonia","Antonin","Antonio","Aphélie","Apollinaire","Apolline","Apollonie","Arcadi","Archibald","Archibaldine","Argine","Ariane","Ariel","Ariste","Aristide","Arlette","Armael","Armaela","Armand","Armande","Armel","Armele","Armelin","Armeline","Armelle","Armilla","Arnaud","Arnaude","Arnold","Arsène","Artémise","Arthur","Arzaela","Arzel","Arzela","Arzhel","Astrid","Astride","Astruc","Athanagor","Athanagore",
                                     "Athanase","Atika","Aubin","Aubry","Aude","Audrey","Audry","Auger","Auguste","Augustin","Augustina","Augustine","Aumérine","Aure","Aurel","Auréle","Aurélia","Aurélian","Auréliane","Aurélianne","Aurélie","Aurélien","Auriane","Aurore","Austin","Axel","Axelle","Aymeric","Babacar","Bali","Balthazar","Baptistette","Barbara","Barbe","Barbon","Barnabé","Barthélémy","Basile","Bastien","Bastienne","Baudouin","Bawer","Béa","Béatrice","Béatrix","Bébert","Bélinda","Bénédicte","Bénigne","Benito","Benjamin","Benoit","Benoît","Bérangère","Bérenger","Bérengère","Bérénice","Bernadette","Bernard","Bernardin","Berthe","Bertrand",
                                     "Beyhan","Bianca","Bibiane","Blaisette","Blanche","Blandine","Boniface","Boris","Boson","Boutros","Bovick","Brian","Brice","Brigitte","Bruce","Bruno","Bryan","Caïn","Calvin","Camélia","Candice","Caprais","Carin","Carina","Carine","Carl","Carla","Carlos","Carmen","Carolane","Carole","Carole Anne","Caroline","Casarie","Casimir","Cassandre","Cassien","Castor","Catherine","Cathy","Cécile","Cécilia","Céciliane","Cédric","Céleste","Célestin","Célestine","Célia","Celian","Célian","Celiane","Céliane","Céline","Césaire","César","Césarion","Chadrack","Chantal",
                                     "Chantale","Charlène","Charles","Charline","Charlotte","Childéric","Chloé","Christelle","Christian","Christiane","Christine","Christophe","Claire","Clairon","Clara","Clarisse","Claudette","Claudia","Claudie","Claudine","Cléa","Cléandre","Clélia","Clélie","Clémence","Clément","Clémentine","Cléopâtre","Cléry","Clio","Cloclo","Clodette","Clotaire","Clothilde","Clotilde","Clovis","Colas","Colette","Colin","Coline","Collart","Collin","Colotte","Côme","Conchita","Conrad","Constant","Constantin","Constantine","Cora","Coralie","Corentin","Corentine","Corinne","Corneille","Cornélie","Cosette","Cosme","Crépin","Cunégonde","Cynthia","Cyr","Cyran","Cyril","Cyrille","Dahlia","Dalhia","Dalila","Damia","Damian",
                                     "Damiana","Damiane","Damiano","Damien","Damienne","Damiette","Daniel","Daniela","Danièle","Daniella","Danielle","Danny","Dany","Daphné","Daphnée","David","Débora","Déborah","Delotte","Delphin","Delphine","Demenge","Denis","Denise","Dennis","Denys","Désiré",
                                     "Désirée","Deslotte","Diamonika","Diane","Didier","Didière","Dimitri","Dodo","Dominic","Domitille","Donald","Donat","Donatien","Dorène","Dorian","Dorine","Doris","Dvora","Dylan","Edgar","Edgard","Édith","Edmond","Edmonde","Edona","Édouard","Edvige","Edwige","Éléonore","Elfi","Éliacin","Eliana","Éliana","Éliane","Élie","Élisa","Élisabeth","Élise","Élisée","Élissande","Élodie","Éloi","Éloïse","Elphie","Elsa","Émeline","Émeric","Émerine","Émérine","Émile","Émilie","Émilien","Éminé","Emma","Emmanuel","Emmanuelle","Émy","Enoha","Enora","Énora","Enrique","Enzo","Éphémie","Éphrem","Epnon","Epvre","Êrane","Éric","Érik","Ermel","Ernest",
                                     "Ernestine","Ervan","Erwan","Erza","Estelle","Estève","Esther","Estienne","Ethan","Éthan","Étienne","Eugène","Eugénie","Eulalie","Euphrosyne","Eusèbe","Eustache","Eutrope","Éva","Évariste","Ève","Évelyne","Ezequiel","Fabien","Fabienne","Fabrice","Fanchette","Fanchon","Fanny","Fatima","Fatimata","Fatoumata","Fébronie","Félicien","Félicité","Félix","Féodor","Ferdinand","Ferdinande","Fernand","Fernande","Fidèle","Fiona","Firmin","Flavie","Flavien","Fleur","Flora","Flore","Florence","Florent","Florentin","Florian","Floriane","Foucault","Fraimbault","France","Francelin","Franceline","Francette","Francine","Francis","Franck","François",
                                     "Françoise","Frank","Fred","Frédégonde","Frédéric","Frédérique","Fulbert","Gabriel","Gabrièle","Gabrielle","Gaël","Gaëla","Gaëlane","Gaële","Gaëlic","Gaëlig","Gaëlla","Gaëllane","Gaëlle","Gaétan","Gaëtan","Gaétane","Gaëtane","Garance","Gaspard","Gaston","Gastonne","Gaultier","Gauthier","Gautier","Gauvain","Gaylord","Gégé","Gélase","Genès","Genest","Genet","Geneviève","Gengoul","Gengoult","Genis","Geoffroy","Georges","Georgette","Georgie","Georgine","Gérald","Géralda","Géralde","Géraldine","Géraldo","Géraldy","Gérard","Géraud","Germain","Germaine","Gertrude","Gervais","Gervaise","Ghislain","Ghislaine","Gilbert","Gilberte","Gilberthe","Gildas","Ginette","Gisèle","Giselle","Gislaine",
                                     "Godefroy","Gontran","Gontrand","Gonzague","Goropius","Gratien","Graziella","Greg","Grégoire","Grégory","Gudule","Guénaël","Guénaèle","Guénaële","Guénaëlle","Guerin","Guilaine","Guilhem","Guillain","Guillaume","Guillaumette","Guillemette","Guislain","Guislaine","Günther","Guoqing","Gustave","Guy","Guylain","Guylaine","Gwenaël","Gwenaëla","Gwenaële","Gwenaëlla","Gwenaëlle","Gwendolen","Gwendoline","Gwendolyne","Gwennaëlle","Habib","Hanne","Harlem","Haroun","Hébert","Hector","Hedwige",
                                     "Hégésippe","Hélène","Helga","Hélie","Hélion","Héloïse","Henri","Henriette","Héraclite","Herbert","Hercule","Héritier","Hermel","Hermelin","Hermeline","Herménégild","Hermione","Hervé","Hilaire","Hilda","Hildegarde","Hippolyte","Homère","Honoré","Honorine","Hortense","Hrant","Hubert","Hugo","Hugues","Huguette","Humbert","Hyacinthe","Hydulphe","Ida","Ignace","Igor","Ilan","Inès","Ingrid","Inna","Irène","Iris","Isa","Isaac","Isabelle","Isaïe","Isaline","Iseult","Iseut","Isidore","Isolde","Issa","Ivan","Ivo","Jac","Jacinthe","Jackson","Jacob","Jacqueline","Jacques","Jacquette","Jacquot","Jade","James","Jamet","Jamette","Jammes","Jason",
                                     "Jean","Jean-Baptiste","Jean-Charles","Jean-Christian","Jean-Christophe","Jean-Claude","Jean-François","Jean-Guy","Jean-Jacques","Jean-Joël","Jean-Louis","Jean-Loup","Jean-Luc","Jean-Marc","Jean-Marie","Jean-Michel","Jeanne","Jeannette","Jeannine","Jean-Noël","Jeannon","Jeannot","Jean-Pascal","Jean-Paul","Jean-Philippe","Jean-Pierre","Jean-Yves","Jefferson","Jehan","Jehanne","Jenique","Jennifer","Jérémie","Jérémy","Jérôme","Jéromine","Jéronime","Jessica","Jésus","Jihu","Ji-hu","Jihun","Ji-hun","Jinping","Joachim","Joanne","Joannie","Joceline","Jocelyn","Jocelyne","Joé","Joël","Joëlle","Johanna","Johanne","Jonas","Jonathan","Jordan","Jordana","Jordane","Jordann","Jordanna","Jordanne",
                                     "Jordany","Jordi","José","Josée","Joseph","Josette","Josiane","Joslin","Josse","Josselin","Josselyne","Josué","Josy","Jourdain","Jourdan","Judas","Jude","Judith","Jules","Julia","Julian","Juliana","Julie","Julien","Julienne","Juliette","Juline","Julius","Junine","Just","Juste","Justin","Justine","Karen","Karin","Karina","Karine","Katia","Katya","Kenji","Kenkichi","Kevin","Kévin","Komachi","Ksenia","Kurdy","Kymie","Ladislas","Ladislaw","Ladislawa","Laeticia","Laetitia","Lala","Lambert","Lambertine","Lambrecht","Lamia","Lara","Larissa","Laszlo","Laura","Laurane","Lauranne","Laure","Laurea","Laureen","Laureline","Laurelle","Lauren","Laurena","Laurence","Laurène","Laurent","Lauretta",
                                     "Laurette","Lauria","Lauriane","Laurianne","Laurice","Laurie","Laurine","Laury","Lauryn","Lauryne","Layla","Lazare","Léa","Léana","Léane","Léanne","Lédy","Léger","Leila","Lena","Léo","Léon","Léonard","Leonarda","Léonid","Léonidas","Léonie","Léonine","Léonore","Léopold","Lesia","Lesya","Lev","Lévi","Lexane","Lia","Libert","Lila","Lili","Lilia","Liliane","Lilliane","Lilou","Lina","Lincoln","Linda","Line","Linus","Lionel","Lisa","Lise","Liselotte","Liseron","Lisette","Livia","Lizier","Loana","Loïc","Lola","Lora","Loréa","Lorelie","Lorella","Lorena","Lorène","Lorenza","Lorenzo","Loretta",
                                     "Lorette","Loria","Loriane","Lorie","Lorine","Lorraine","Lory","Lothaire","Louis","Louise","Louisette","Louis-Philippe","Louiza","Loulou","Louna","Loup","Luc","Luca","Lucas","Luce","Luciano","Lucie","Lucien","Lucienne","Lucile","Lucille","Lucrèce","Ludovic","Ludovine","Luitvin","Lycaon","Lydia","Lydie","Macaire","Maclou","Madeleine","Madelon","Maëlle","Maëlys","Maeva","Maéva","Maèva","Maëva","Magali","Magalie","Magaly","Magdaléna","Maguelone","Maguelonne","Mahamadou","Mahé","Maheva","Mahmoud","Mai","Maïa","Maialen","Mailis","Maïssa","Maïté","Maïtena","Maleaume","Malika","Malo","Mamadou","Manami","Manon","Manuel","Manuela","Manuella","Mara","Marc","Marcel","Marcelin","Marceline","Marcelle",
                                     "Marcellin","Marcien","Margaret","Margarita","Margault","Margot","Margoton","Marguerite","Mari","Maria","Mariana","Marianne","Marie","Marie-Ange","Marie-Chantal","Marie-Christine","Marie-Claire","Marie-Claude","Marie-Élise","Marie-Ève","Marie-France","Marie-Françoise","Marie-Gisèle","Marie-Hélène","Marie-José","Marie-Laure","Marielle","Marie-Madeleine","Marie-Noëlle","Marie-Paule","Marie-Pier","Marie-Pierre","Marie-Thérèse","Marietta","Mariette","Marina","Marine","Marinette","Mario","Marion","Marius","Marjolain","Marjolaine","Marlaine","Marlène","Marlenne","Marleyne","Marthe","Martial","Martin","Martine","Martinien","Marty","Marye","Maryne","Maryvette","Maryvonne","Matéo","Mathéo",
                                     "Mathias","Mathieu","Mathilde","Mathis","Mathurin","Mathurine","Mathys","Matisse","Matthias","Matthieu","Matthis","Maud","Maude","Maurice","Mauricette","Max","Maxim","Maximilien","Maximin","Maxine","Mayeul","Mayeule","Maÿlice","Mayline","Maylis","Maÿliss","Mayssa","Maÿssa","Médard","Megan","Mégane","Mehdi","Melaine","Mélanie","Mélany","Melchior","Mélina","Méline","Mélinée","Mélissa","Mélodie","Mélodine","Mélody","Melvin","Melvine","Melvyn","Melvyne","Melvynn","Mélyne","Melyssa","Mercedes","Mercédès","Merlin","Merline","Méthode","Micaël","Michael","Michaël","Michaeli","Michaelis","Michel","Michèle","Micheline","Michelle","Michka","Mickael","Mickaël","Mikaël","Mikhael","Mikhaylov","Milène",
                                     "Milo","Mimile","Minjun","Min-jun","Minseo","Min-seo","Mira","Mireille","Miriam","Miroslav","Miroslava","Miryam","Mitch","Mitchell","Mithridate","Modestine","Moïra","Momo","Monika","Monique","Morgane","Morvan","Muriel","Murielle","Myriam","Nabil","Nabor","Nacer","Nadège","Nadia","Nadine","Nahel","Nana","Nanard","Naomi","Naomie","Narcisse","Natacha","Natasha","Nathalie","Nathan","Nathanaël","Nathanaèle","Nathanaëlle","Nathaniel","Nathanielle","Nazaire","Nestor","Nicaise","Nicéphore","Nico","Nicodème","Nicolas","Nicole","Nicolette","Nicomède","Nina","Ninon","Noam","Noël","Noëlle","Noémia","Noémie","Norbert","Océane","Octave","Octavie","Octavien","Odet","Odette","Odile","Odon","Ogier","Oleg",
                                     "Olga","Olive","Olivia","Olivier","Olympe","Omar","Ombeline","Ombline",
                                     "Omer","Omérine","Ondine","Onésime","Ophélia","Ophélie","Oriana","Oriane","Orianne","Ornella","Oscar","Oswald","Othon","Pablo","Paco","Pacôme","Paola","Paolo","Parker","Pascal","Pascale","Pascaline","Patrice","Patricia","Patrick","Paul","Paula","Paule","Paulette","Paulin","Paulina","Pauline","Paulo","Paul-Olivier","Pavel","Pavlina","Peggy","Peio","Pélage","Pénélope","Périne","Périnne","Pernelle","Peronnelle","Péronnelle","Perrette","Perrin","Perrine","Perryne","Persian","Peter","Pétra","Pétronille","Philibert","Philiberte","Philippe","Philippine","Philippon","Philippoté","Philippotée","Philomène","Pierre","Pierre-Louis","Pierre-Marie","Pierre-Olivier","Pierrette","Pierre-Yves","Pierrot",
                                     "Priscille","Priscillia","Procope","Prosper","Prudence","Prune","Quentin","Rachel","Rachelle","Rachida","Radegonde","Rafael","Rafaela","Rafaèla","Rafaele","Rafaèle","Rafaëlla","Rafaëlle","Raoul","Raphaël","Raphaèla","Raphaèle","Raphaëlla","Raphaëlle","Raymond","Raymonde","Réal","Rebecca","Rébecca","Régina","Régine","Régis","Reine","Réjane","Réjeanne","Rémy","Renan","Renaud","René","Renée","Reuel","Reynald","Ricardo","Richard","Rita","Robert","Roberta","Roberte","Robin","Roch","Rodolphe","Rodrigue","Roger","Roland","Rolande","Romain","Romaine","Roman","Romane","Roméo","Romuald","Romualde","Ronan","Rosa","Rosalie","Rosange","Rose","Roseline","Roselyne","Rose-Marie","Rosette","Rosine","Roxane",
                                     "Roxanne","Roy","Ruben","Ruth","Sabin","Sabina","Sabine","Sabrina","Sabrine","Sacha","Sam","Samantha","Sami","Sammy","Samson","Samuel","Sandra","Sandrine","Sarah","Satomi","Saturnin","Sauvan","Sauvane","Savinien","Sébastien","Ségolaine","Ségolène","Selma","Sémion","Sémione","Seohyeon","Seoyeon","Séphora","Séraphin","Séraphine","Serge","Sergueï","Servais","Sever","Séverin","Séverine","Shinji","Sid","Sidoine","Sidonie","Sigismond","Sigolène","Sigurdur","Silva","Silvain","Silvaine","Silvère","Simon","Simona","Simone","Soheil","Solange","Solène","Solenn","Solenna","Solenne","Soline","Solveig","Sonia","Soohyun","Sophie","Sören","Søren","Souleymane","Stanislas","Stefani","Stéphanie","Steve","Steven",
                                     "Suzan","Suzanne","Sylvain","Sylvaine","Sylvane","Sylvanie","Sylvestre","Sylvia","Sylviane","Sylvie","Symphorien","Tancrède","Tanguy","Tania","Tanya","Tariq","Tatiana","Thaddée","Thadée","Thaïs","Théa","Thècle","Théo","Théodora","Théodose","Théodule","Théophile","Théophraste","Thérèse","Thibault","Thiébaud","Thierry","Thimoté","Thimothé","Thomas","Tiffany","Tilio","Timothé","Tino","Tiphaine","Tite","Titien","Tobie","Tognon","Tom","Tomoko","Tony","Toto","Totor","Toussaint","Trémeur","Trésorine","Tristan","Tristane","Trond","Trudon","Tugdual","Typhaine","Ulysse","Urbain","Urbaine","Ursula","Ursule","Vaast","Valburge","Valentin","Valentine","Valère","Valérie","Valérien","Valéry","Vallier","Vanessa",
                                     "Venceslas","Véran","Véro","Véronique","Victoire","Victor","Victoria","Victorien","Vidal","Vincent","Vinciane","Violaine","Violette","Virgile","Virginie","Viviane","Vivien","Vladimir","Vladislas","Vladislav","Walter","Walther","Warner","Washington","Wenceslas","Wilfrid","Wilfried","William","Wilson","Wladislav","Xavier","Xavière","Xénie","Yann","Yoann","Yolande","Yolène","Yseult","Yuna","Yves","Yvette","Yvon","Yvonne","Zacchée","Zacharie","Zachary","Zachée","Zazie","Zoé","Zoltan"};


PCx_Tree::PCx_Tree(unsigned int treeId):treeId(treeId)
{
    if(treeId==0)
    {
        qFatal("Assertion failed");
    }
    QSqlQuery query;

    query.prepare("SELECT nom,termine,le_timestamp from index_arbres where id=:id");
    query.bindValue(":id",treeId);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        die();
    }
    if(query.next())
    {
        this->treeName=query.value("nom").toString();
        this->treeId=treeId;
        this->finished=query.value("termine").toBool();
        this->creationTime=query.value("le_timestamp").toString();
    }
    else
    {
        qCritical()<<"Invalid tree ID"<<treeId;
        die();
    }

    loadTypesFromDb();
}

unsigned int PCx_Tree::addNode(unsigned int pid, unsigned int typeId, const QString &name, unsigned int forcedNodeId)
{
    if(pid==0||typeId==0||name.isEmpty()||name.size()>MAXNODENAMELENGTH ||
             getNumberOfNodes()>MAXNODES)
    {
        qFatal("Assertion failed");
    }

    if(forcedNodeId==1)
    {
        qFatal("forcedNodeId cannot be 1");
    }



    QSqlQuery q;

    if(forcedNodeId==0)
    {
        q.prepare(QString("insert into arbre_%1 (nom,pid,type) values (:nom, :pid, :type)").arg(treeId));
        q.bindValue(":nom",name);
        q.bindValue(":pid",pid);
        q.bindValue(":type",typeId);
    }
    else
    {
        q.prepare(QString("insert into arbre_%1 (id,nom,pid,type) values (:id,:nom, :pid, :type)").arg(treeId));
        q.bindValue(":id",forcedNodeId);
        q.bindValue(":nom",name);
        q.bindValue(":pid",pid);
        q.bindValue(":type",typeId);
    }


    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();

    }

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }

    return q.lastInsertId().toUInt();
}

unsigned int PCx_Tree::getTypeId(unsigned int nodeId) const
{
    QSqlQuery q;
    q.prepare(QString("select type from arbre_%1 where id=:idnode").arg(treeId));
    q.bindValue(":idnode",nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.next())
    {
        qWarning()<<"Missing node";
        return 0;
    }
    return q.value(0).toUInt();
}


bool PCx_Tree::updateNode(unsigned int nodeId, const QString &newName, unsigned int newType)
{
    if(newName.isEmpty()||nodeId==0||
            newName.size()>MAXNODENAMELENGTH||
            !isTypeIdValid(newType))
    {
        qFatal("Assertion failed");
    }

    QSqlQuery q;

    //Special consideration for root does not update the type
    if(nodeId==1)
    {
        q.prepare(QString("update arbre_%1 set nom=:nom where id=:id").arg(treeId));
        q.bindValue(":nom",newName);
        q.bindValue(":id",nodeId);
    }

    else
    {
        q.prepare(QString("update arbre_%1 set nom=:nom, type=:type where id=:id").arg(treeId));
        q.bindValue(":nom",newName);
        q.bindValue(":type",newType);
        q.bindValue(":id",nodeId);
    }

    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }

    return true;
}





QStringList PCx_Tree::getListOfCompleteNodeNames() const
{
    QStringList nodeNames;
    QSqlQuery q;
    if(!q.exec(QString("select id,nom,type from arbre_%1").arg(treeId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    while(q.next())
    {
        nodeNames.append(QString("%3. %1 %2")
                         .arg(idTypeToName(q.value("type").toUInt()),
                         q.value("nom").toString())
                         .arg(q.value("id").toUInt()));

    }
    return nodeNames;
}


QList<unsigned int> PCx_Tree::getLeavesId()
{
    QList<unsigned int> leaves;
    QList<unsigned int> nodes;
    nodes=getNodesId();
    foreach (unsigned int node, nodes)
    {
        if(isLeaf(node))
        {
            leaves.append(node);
        }
    }
    //qDebug()<<"Leaves for tree "<<treeId<< " = "<<leaves;
    return leaves;
}


QList<unsigned int> PCx_Tree::getAncestorsId(unsigned int node) const
{
    QList<unsigned int> ancestors;
    if(node==1)return ancestors;

    do
    {
        unsigned int parent=getParentId(node);
        ancestors.append(parent);
        node=parent;
    }while(node>1);
    return ancestors;
}

int PCx_Tree::guessHierarchy()
{
    QSqlQuery q;
    QHash<QString,unsigned int>nameToId;
    QHash<QString,unsigned int>prefixToId;
    QHash<unsigned int,unsigned int> idToPid,oldIdToPid;
    if(!q.exec(QString("select * from arbre_%1 where id>1").arg(treeId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    QRegExp numRegExp("^(\\d+)");
    while(q.next())
    {
        QString nom=q.value("nom").toString();
        unsigned int id=q.value("id").toUInt();
        unsigned int pid=q.value("pid").toUInt();

        //Do not allow duplicates, only takes the first (same name, different type)
        if(!nameToId.contains(nom))
        {
            nameToId.insert(nom,id);
            oldIdToPid.insert(id,pid);
            int pos=numRegExp.indexIn(nom);
            if(pos>-1)
            {
                prefixToId.insert(numRegExp.cap(1),id);
            }
        }
    }
    if(prefixToId.count()<=1)
        return 0;


    for(QHash<QString,unsigned int>::const_iterator it=prefixToId.constBegin(),end=prefixToId.constEnd();it!=end;++it)
    {
    //foreach(const QString &key,prefixToId.keys())
    //{
        if(it.key().size()>1)
        {
            //TODO: multi level chop to find 701 child of 7 without 70
            QString chopped=it.key();
            chopped.chop(1);
            if(prefixToId.contains(chopped))
            {
                idToPid.insert(it.value(),prefixToId.value(chopped));
            }
        }
    }

    int count=0;
    QSqlDatabase::database().transaction();

    for(QHash<unsigned int,unsigned int>::const_iterator it=idToPid.constBegin(),end=idToPid.constEnd();it!=end;++it)
    {
        unsigned int id=it.key();

        if(oldIdToPid.value(id)!=it.value())
        {
            QSqlQuery q;
            q.prepare(QString("update arbre_%1 set pid=:pid where id=:id").arg(treeId));
            q.bindValue(":pid",it.value());
            q.bindValue(":id",id);
            if(!q.exec())
            {
                qCritical()<<q.lastError();
                die();
            }
            if(q.numRowsAffected()!=1)
            {
                qCritical()<<q.lastError();
                die();
            }
            count++;
        }
    }
    QSqlDatabase::database().commit();

    return count;

}

QList<unsigned int> PCx_Tree::getDescendantsId(unsigned int node)
{
    QList<unsigned int> children;
    QList<unsigned int> descendants;
    if(isLeaf(node))
    {
        return descendants;
    }
    children=getChildren(node);
    foreach(unsigned int child,children)
    {
        descendants.append(child);
        descendants.append(getDescendantsId(child));
    }
    return descendants;
}


QList<unsigned int> PCx_Tree::getLeavesId(unsigned int node)
{
    QList<unsigned int> children;
    QList<unsigned int> leaves;
    if(isLeaf(node))
    {
        leaves.append(node);
        return leaves;
    }
    children=getChildren(node);
    foreach(unsigned int child,children)
    {
        if(isLeaf(child))
        {
            leaves.append(child);
        }
        else
        {
            leaves.append(getLeavesId(child));
        }
    }
    return leaves;
}


QSet<unsigned int> PCx_Tree::getNodesWithSharedName() const
{
    QSqlQuery q(QString("select * from arbre_%1").arg(treeId));
    QSet<unsigned int> nodes;
    QHash<QString,QPair<unsigned int,unsigned int> > nameToTypeAndId;
    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        die();
    }
    while(q.next())
    {
        QString name=q.value("nom").toString().toLower();
        if(nameToTypeAndId.contains(name))
        {
            nodes.insert(q.value("id").toUInt());
            nodes.insert(nameToTypeAndId.value(name).second);
        }
        else
        {
            QPair<unsigned int,unsigned int>typeAndId;
            typeAndId.first=q.value("type").toUInt();
            typeAndId.second=q.value("id").toUInt();
            nameToTypeAndId.insert(name,typeAndId);
        }
    }
    return nodes;
}

bool PCx_Tree::isLeaf(unsigned int nodeId)
{
    if(nodeId==0)
    {
        qFatal("Assertion failed");
    }
    if(finished && nodeIsALeaf.contains(nodeId))
    {
        return nodeIsALeaf.value(nodeId);
    }

    else
    {
        QSqlQuery q;
        q.prepare(QString("select count(*) from arbre_%1 where pid=:nodeid").arg(treeId));
        q.bindValue(":nodeid",nodeId);
        if(!q.exec())
        {
            qCritical()<<q.lastError();
            die();
        }
        if(q.next())
        {
            if(q.value(0).toInt()==0)
            {
                if(finished)
                    nodeIsALeaf.insert(nodeId,true);
                return true;
            }
            else
            {
                if(finished)
                    nodeIsALeaf.insert(nodeId,false);
                return false;
            }
        }
    }
    return false;
}


unsigned int PCx_Tree::getNumberOfNodesWithThisType(unsigned int typeId) const
{
    if(typeId==0)
    {
        qFatal("Assertion failed");
    }
    QSqlQuery q;
    q.prepare(QString("select count(*) from arbre_%1 where type=:typeId").arg(treeId));
    q.bindValue(":typeId",typeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.next())
        return 0;
    return q.value(0).toUInt();
}

QList<unsigned int> PCx_Tree::sortNodesBFS(QList<unsigned int> &nodes) const
{
    QList<unsigned int>sortedNodes;

    QList<unsigned int>toCheck;
    toCheck.append(1);

    while(!toCheck.isEmpty())
    {
        unsigned int node=toCheck.takeFirst();
        if(nodes.contains(node))
            sortedNodes.append(node);
        toCheck.append(getChildren(node));
    }
    return sortedNodes;
}


QList<unsigned int> PCx_Tree::sortNodesDFS(QList<unsigned int> &nodes,unsigned int currentNode) const
{
    QList<unsigned int> sortedNodes;

    if(nodes.contains(currentNode))
        sortedNodes.append(currentNode);

    QList<unsigned int> children=getChildren(currentNode);
    foreach(unsigned int childId,children)
    {
        sortedNodes.append(sortNodesDFS(nodes,childId));
    }
    return sortedNodes;
}

unsigned int PCx_Tree::getParentId(unsigned int nodeId) const
{
    QSqlQuery q;
    q.prepare(QString("select pid from arbre_%1 where id=:nodeid").arg(treeId));
    q.bindValue(QStringLiteral(":nodeid"),nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }
    if(q.next())
    {
        return q.value(0).toUInt();
    }
    return 0;
}

QList<unsigned int> PCx_Tree::getChildren(unsigned int nodeId) const
{
    QList<unsigned int> listOfChildren;
    QSqlQuery q;
    q.prepare(QString("select id from arbre_%1 where pid=:nodeid order by nom").arg(treeId));
    q.bindValue(QStringLiteral(":nodeid"),nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    while(q.next())
    {
        listOfChildren.append(q.value(0).toUInt());
    }
    return listOfChildren;
}



void PCx_Tree::updateNodePid(unsigned int nodeId, unsigned int newPid)
{
    if(nodeId<=1 || newPid == 0)
    {
        qFatal("Assertion failed");
    }
    QSqlQuery q;

    q.prepare(QString("update arbre_%1 set pid=:pid where id=:id").arg(treeId));
    q.bindValue(QStringLiteral(":pid"),newPid);
    q.bindValue(QStringLiteral(":id"),nodeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()!=1)
    {
        qCritical()<<q.lastError();
        die();
    }
}

bool PCx_Tree::finishTree()
{
    if(this->finished==false)
    {
        QSqlQuery q;
        q.prepare("update index_arbres set termine=1 where id=:id");
        q.bindValue(":id",treeId);
        if(!q.exec())
        {
            qCritical()<<q.lastError();
            die();
        }
        if(q.numRowsAffected()!=1)
        {
            qCritical()<<q.lastError();
            die();
        }
        this->finished=true;
    }
    return true;
}





bool PCx_Tree::nodeExists(const QString &name, unsigned int typeId) const
{
    QSqlQuery q;

    q.prepare(QString("select count(*) from arbre_%1 where nom=:nom and type=:type").arg(treeId));
    q.bindValue(QStringLiteral(":nom"),name);
    q.bindValue(QStringLiteral(":type"),typeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.next())
    {
        if(q.value(0).toInt()>0)
        {
            return true;
        }
    }
    return false;
}

int PCx_Tree::getNodeIdFromTypeAndNodeName(const QPair<QString, QString> &typeAndNodeName) const
{
    int typeId=nameToIdType(typeAndNodeName.first);
    if(typeId==-1)
    {
        qWarning()<<"Unable to find the node identifier";
        return -1;
    }
    QSqlQuery q;
    QString nodeNameSpl=typeAndNodeName.second.simplified();
    q.prepare(QString("select id from arbre_%1 where nom=:nameId and type=:typeId").arg(treeId));
    q.bindValue(QStringLiteral(":nameId"),nodeNameSpl);
    q.bindValue(QStringLiteral(":typeId"),typeId);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        return -1;
    }
    if(!q.next())
    {
        qWarning()<<"Unable to find the node identifier";
        return -1;
    }
    return q.value(0).toInt();
}


QPair<QString,QString> PCx_Tree::getTypeNameAndNodeName(unsigned int node) const
{
    if(node==0)
    {
        qFatal("Assertion failed");
    }
    QSqlQuery q;
    q.prepare(QString("select type,nom from arbre_%1 where id=:id").arg(treeId));
    q.bindValue(QStringLiteral(":id"),node);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    QPair<QString,QString> typeNameAndNodeName;

    if(q.next())
    {
        if(node>1)
        {
            QString typeName=idTypeToName(q.value(0).toUInt());
            typeNameAndNodeName.first=typeName;
            typeNameAndNodeName.second=q.value(1).toString();
        }
        //Root does not has type
        else
        {
            typeNameAndNodeName.first=QString();
            typeNameAndNodeName.second=q.value(1).toString();
        }
        return typeNameAndNodeName;
    }
    else
    {
        qWarning()<<"Missing node"<<node;
    }
    return QPair<QString,QString>();

}


QString PCx_Tree::getNodeName(unsigned int node) const
{
    if(node==0)
    {
        qFatal("Assertion failed");
    }
    QSqlQuery q;
    q.prepare(QString("select type,nom from arbre_%1 where id=:id").arg(treeId));
    q.bindValue(QStringLiteral(":id"),node);
    if(!q.exec())
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.next())
    {
        if(node>1)
        {
            QString typeName=idTypeToName(q.value(0).toUInt());
            return QString("%3. %1 %2").arg(typeName,q.value(1).toString()).arg(node);
        }
        //Root does not has type
        else
        {
            return QString::number(node)+". "+q.value(1).toString();
        }
    }
    else
    {
        qWarning()<<"Missing node"<<node;
    }
    return QString();
}


bool PCx_Tree::deleteNode(unsigned int nodeId)
{
    if(nodeId==0)
    {
        qFatal("Assertion failed");
    }
    QList<unsigned int> listOfChildrens;
    QSqlQuery q;
    q.prepare(QString("select id from arbre_%1 where pid=:pid").arg(treeId));
    q.bindValue(":pid",nodeId);
    q.exec();

    if(!q.isActive())
    {
        qCritical()<<q.lastError();
        return false;
    }
    while(q.next())
    {
        listOfChildrens.append(q.value(0).toUInt());
    }

    if(listOfChildrens.size()==0)
    {
        q.prepare(QString("delete from arbre_%1 where id=:id").arg(treeId));
        q.bindValue(":id",nodeId);
        if(!q.exec())
        {
            qCritical()<<q.lastError();
            die();
        }

        if(q.numRowsAffected()!=1)
        {
            qCritical()<<q.lastError();
            die();
        }
        return true;
    }
    else
    {
        foreach(unsigned int child,listOfChildrens)
        {
            deleteNode(child);
        }
        deleteNode(nodeId);
    }
    return true;
}



QString PCx_Tree::toDot() const
{
    QList<unsigned int> nodes=getNodesId();

    QString out=QStringLiteral("graph g{\nrankdir=LR;\n");

    foreach(unsigned int node,nodes)
    {
        //Escape double quotes for dot format
        out.append(QString("%1 [label=\"%2\"];\n").arg(node).arg(getNodeName(node).replace('"',QString("\\\""))));
    }

    foreach(unsigned int node,nodes)
    {
        unsigned int pid=getParentId(node);

        if(pid!=0)
        {
            out.append(QString("\t%1--%2;\n").arg(pid).arg(node));
        }
    }
    out.append(QStringLiteral("}\n"));
    return out;
}

bool PCx_Tree::toXLSX(const QString &fileName) const
{
    if(fileName.isEmpty())
    {
        qFatal("Assertion failed");
    }
    QList<unsigned int> nodes=getNodesId();

    QXlsx::Document xlsx;

    xlsx.write(1,1,"Identifiant du noeud");
    xlsx.write(1,2,"Type du noeud");
    xlsx.write(1,3,"Nom du noeud");
    xlsx.write(1,4,"Identifiant du père (1=racine)");

    QPair<QString,QString>typeNameAndNodeName;

    int row=2;
    foreach(unsigned int node,nodes)
    {
        //Skip the root
        if(node==1)
            continue;

        unsigned int pid=getParentId(node);
        typeNameAndNodeName=getTypeNameAndNodeName(node);

        xlsx.write(row,1,node);
        xlsx.write(row,2,typeNameAndNodeName.first);
        xlsx.write(row,3,typeNameAndNodeName.second);
        xlsx.write(row,4,pid);
        row++;


    }
    return xlsx.saveAs(fileName);
}

void PCx_Tree::loadTypesFromDb()
{
    idTypesToNames.clear();
    listOfTypeNames.clear();
    QSqlQuery query;
    if(!query.exec(QString("select * from types_%1").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }

    while(query.next())
    {
        QString typeName=query.value(1).toString();
        idTypesToNames.insert(query.value(0).toUInt(),typeName);
        listOfTypeNames.append(typeName);
    }
}

int PCx_Tree::_internalAddTree(const QString &name,bool createRoot)
{
    QSqlQuery query;

    query.prepare("insert into index_arbres (nom) values(:nom)");
    query.bindValue(":nom",name);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError();
        die();
    }

    QVariant lastId=query.lastInsertId();

    if(!lastId.isValid())
    {
        qCritical()<<"Invalid last inserted Id";
        die();
    }
    int treeId=lastId.toInt();

    if(!query.exec(QString("create table arbre_%1(id integer primary key autoincrement, "
                           "nom text not null, pid integer not null,"
                           " type integer references types_%1(id) on delete restrict)").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError();
        die();
    }

    if(!query.exec(QString("create index idx_arbre_%1_pid on arbre_%1(pid)").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }

    if(!query.exec(QString("create table types_%1(id integer primary key autoincrement, nom text unique not null)").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError();
        die();
    }

    if(createRoot)
    {
        if(!query.exec(QString("insert into arbre_%1 (nom,pid,type) values ('Racine',0,NULL)").arg(treeId)))
        {
            qCritical()<<query.lastError();
            die();
        }
        if(query.numRowsAffected()!=1)
        {
            qCritical()<<query.lastError();
            die();
        }
    }
    return lastId.toInt();
}


bool PCx_Tree::validateType(const QString &newType)
{
    // qDebug()<<"Type to validate = "<<newType;
    if(newType.size()>MAXNODENAMELENGTH)
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Nom trop long !"));
        return false;
    }

    if(newType.isEmpty())
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Vous ne pouvez pas utiliser un type vide !"));
        return false;
    }
    else if(listOfTypeNames.contains(newType))
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Le type <b>%1</b> existe déjà !").arg(newType.toHtmlEscaped()));
        return false;
    }
    return true;
}

int PCx_Tree::nameToIdType(const QString &typeName) const
{
    if(typeName.isEmpty())
    {
        qFatal("Assertion failed");
    }
    QString typeNameSpl=typeName.simplified();
    if(listOfTypeNames.contains(typeNameSpl))
        return idTypesToNames.key(typeNameSpl);
    else return -1;
}

QList<QPair<unsigned int, QString> > PCx_Tree::getAllTypes() const
{
    QList<QPair<unsigned int,QString> > types;
    QSqlQuery query;
    if(!query.exec(QString("select * from types_%1").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }

    while(query.next())
    {
        QPair<unsigned int,QString> p;
        p.first=query.value(0).toUInt();
        p.second=query.value(1).toString();
        types.append(p);
    }
    return types;
}

unsigned int PCx_Tree::addType(const QString &typeName)
{
    unsigned int typeId=0;

    if(validateType(typeName)==false)
        return 0;

    else
    {
        QSqlQuery q;
        q.prepare(QString("insert into types_%1 (nom) values(:nom)").arg(treeId));
        q.bindValue(":nom",typeName);
        if(!q.exec())
        {
            qCritical()<<q.lastError();
            die();
        }
        if(q.numRowsAffected()!=1)
        {
            qCritical()<<q.lastError();
            die();
        }

        typeId=q.lastInsertId().toUInt();

        idTypesToNames.insert(typeId,typeName);
        listOfTypeNames.append(typeName);
    }
    return typeId;
}

bool PCx_Tree::deleteType(unsigned int typeId)
{
    if(typeId==0)
    {
        qFatal("Assertion failed");
    }

    QSqlQuery query;

    query.prepare(QString("select count(*) from arbre_%1 where type=:type").arg(treeId));
    query.bindValue(":type",typeId);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        die();
    }

    if(!query.isActive())
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.next())
    {
        //qDebug()<<"Number of nodes of type "<<typeId<<" = "<<query.value(0).toInt();
        if(query.value(0).toInt()>0)
        {
            QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe des noeuds de ce type dans l'arbre. Supprimez-les d'abord, ou changez leur type."));
            return false;
        }

    }
    else return false;

    query.prepare(QString("delete from types_%1 where id=:id").arg(treeId));
    query.bindValue(":id",typeId);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError();
        die();
    }
    loadTypesFromDb();
    return true;
}

unsigned int PCx_Tree::getNumberOfNodes() const
{
    QSqlQuery q;
    if(!q.exec(QString("select count(*) from arbre_%1").arg(treeId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    if(!q.next())
    {
        qCritical()<<q.lastError();
        die();
    }
    return q.value(0).toUInt();
}

QList<unsigned int> PCx_Tree::getNodesId() const
{
    QSqlQuery q;
    QList<unsigned int> nodeIds;
    if(!q.exec(QString("select id from arbre_%1").arg(treeId)))
    {
        qCritical()<<q.lastError();
        die();
    }
    while(q.next())
    {
        nodeIds.append(q.value(0).toUInt());
    }
    return nodeIds;
}

int PCx_Tree::duplicateTree(const QString &newName)
{
    if(newName.isEmpty()||treeNameExists(newName)||newName.size()>MAXOBJECTNAMELENGTH)
    {
        qFatal("Assertion failed");
    }
    QSqlQuery q;

    QSqlDatabase::database().transaction();
    int newTreeId=_internalAddTree(newName,false);
    if(newTreeId<=0)
    {
        qFatal("Assertion failed");
    }

    if(!q.exec(QString("insert into types_%1 select * from types_%2").arg(newTreeId).arg(treeId)))
    {
        qCritical()<<q.lastError();
        die();
    }

    if(q.numRowsAffected()<=0)
    {
        qCritical()<<q.lastError();
        die();
    }

    q.exec(QString("insert into arbre_%1 select * from arbre_%2").arg(newTreeId).arg(treeId));
    if(q.numRowsAffected()<=0)
    {
        qCritical()<<q.lastError();
        die();
    }
    QSqlDatabase::database().commit();
    return newTreeId;
}

QStringList PCx_Tree::getListOfDefaultTypes()
{
    QStringList listOfTypes;
    listOfTypes<<QObject::tr("Maire adjoint")<<QObject::tr("Conseiller")<<QObject::tr("Division")<<QObject::tr("Service");
    return listOfTypes;
}




int PCx_Tree::createRandomTree(const QString &name,unsigned int nbNodes)
{
    if(name.isEmpty() || nbNodes==0 || nbNodes>PCx_Tree::MAXNODES
            || name.size()>MAXOBJECTNAMELENGTH || treeNameExists(name))
    {
        qFatal("Assertion failed");
    }

    int lastId=addTree(name);

    if(lastId<=0)
    {
        qFatal("Assertion failed");
    }

    qsrand(time(NULL));
    unsigned int maxNumType=getListOfDefaultTypes().size();

    QStringList usedFirstNames;
    QSqlDatabase::database().transaction();
    int firstNameListSize=firstNameList.size();
    for(unsigned int i=1;i<nbNodes;i++)
    {
        QSqlQuery q;
        unsigned int type=(qrand()%maxNumType)+1;
        unsigned int pid=(qrand()%i)+1;
        //QString name=QUuid::createUuid().toString();
        QString name=firstNameList.at(qrand()%firstNameListSize);
        while(usedFirstNames.contains(name))
        {
            name=firstNameList.at(qrand()%firstNameListSize);
        }
        usedFirstNames.append(name);
        q.prepare(QString("insert into arbre_%1 (nom,pid,type) values (:nom, :pid, :type)").arg(lastId));
        q.bindValue(QStringLiteral(":nom"),name);
        q.bindValue(QStringLiteral(":pid"),pid);
        q.bindValue(QStringLiteral(":type"),type);
        q.exec();

        if(q.numRowsAffected()!=1)
        {
            qCritical()<<q.lastError();
            deleteTree(lastId);
            die();
        }
    }

    //Reorder types

    PCx_Tree tree(lastId);
    QList<unsigned int>nodes;

    bool changed;
    do
    {
        nodes=tree.getNodesId();
        changed=false;
        foreach(unsigned int node, nodes)
        {
            if(node>1)
            {
                unsigned int type1=tree.getTypeId(node);
                unsigned int pid=tree.getParentId(node);
                unsigned int pidType=tree.getTypeId(pid);
                if(pidType>type1)
                {
                    changed=true;
                    QSqlQuery q;
                    q.prepare(QString("update arbre_%1 set type=:type where id=:id").arg(lastId));
                    q.bindValue(QStringLiteral(":type"),pidType);
                    q.bindValue(QStringLiteral(":id"),node);
                    q.exec();
                    if(q.numRowsAffected()!=1)
                    {
                        qCritical()<<q.lastError();
                        deleteTree(lastId);
                        die();
                    }
                    q.bindValue(QStringLiteral(":type"),type1);
                    q.bindValue(QStringLiteral(":id"),pid);
                    q.exec();
                    if(q.numRowsAffected()!=1)
                    {
                        qCritical()<<q.lastError();
                        deleteTree(lastId);
                        die();
                    }
                }
            }
        }
    }while(changed==true);
    QSqlDatabase::database().commit();
    return lastId;
}


int PCx_Tree::addTree(const QString &name)
{
    QSqlQuery query;

    if(treeNameExists(name))
    {
        QMessageBox::warning(nullptr,QObject::tr("Attention"),QObject::tr("Il existe déjà un arbre portant ce nom !"));
        return -1;
    }

    QSqlDatabase::database().transaction();
    int treeId=_internalAddTree(name,true);
    if(treeId<=0)
    {
        qCritical()<<"Error";
        die();
    }

    QStringList listOfTypes=getListOfDefaultTypes();
    foreach(const QString & oneType,listOfTypes)
    {
        query.prepare(QString("insert into types_%1 (nom) values(:nomtype)").arg(treeId));
        query.bindValue(":nomtype",oneType);
        if(!query.exec())
        {
            qCritical()<<query.lastError();
            die();
        }
        if(query.numRowsAffected()!=1)
        {
            qCritical()<<query.lastError();
            die();
        }
    }
    QSqlDatabase::database().commit();
    return treeId;
}

QList<unsigned int> PCx_Tree::getListOfTreesId(bool finishedOnly)
{
    QList<unsigned int> listOfTrees;

    QSqlQuery query;

    if(!query.exec("select * from index_arbres order by datetime(le_timestamp)"))
    {
        qCritical()<<query.lastError();
        die();
    }

    while(query.next())
    {
        if(query.value("termine").toBool()==true)
        {
            listOfTrees.append(query.value("id").toUInt());
        }
        else if(finishedOnly==false)
        {
            listOfTrees.append(query.value("id").toUInt());
        }
    }
    return listOfTrees;
}

QList<QPair<unsigned int, QString> > PCx_Tree::getListOfTrees(bool finishedOnly)
{
    QList<QPair<unsigned int,QString> > listOfTrees;
    QDateTime dt;

    QSqlQuery query;

    if(!query.exec("select * from index_arbres order by datetime(le_timestamp)"))
    {
        qCritical()<<query.lastError();
        die();
    }

    while(query.next())
    {
        QString item;

        dt=QDateTime::fromString(query.value(3).toString(),"yyyy-MM-dd hh:mm:ss");
        dt.setTimeSpec(Qt::UTC);
        QDateTime dtLocal=dt.toLocalTime();

        if(query.value("termine").toBool()==true)
        {
            item=QString("%1 - %2 (arbre terminé)").arg(query.value(1).toString(),dtLocal.toString(Qt::SystemLocaleShortDate));
            QPair<unsigned int, QString> p;
            p.first=query.value(0).toUInt();
            p.second=item;
            listOfTrees.append(p);
        }
        else if(finishedOnly==false)
        {
            item=QString("%1 - %2").arg(query.value(1).toString(),dtLocal.toString(Qt::SystemLocaleShortDate));
            QPair<unsigned int, QString> p;
            p.first=query.value(0).toUInt();
            p.second=item;
            listOfTrees.append(p);
        }
    }
    return listOfTrees;
}


bool PCx_Tree::treeNameExists(const QString &name)
{
    if(name.isEmpty())
    {
        qFatal("Assertion failed");
    }
    QSqlQuery query;

    query.prepare("select count(*) from index_arbres where nom=:name");
    query.bindValue(":name",name);
    if(!query.exec())
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.next())
    {
        if(query.value(0).toInt()>0)
        {
            return true;
        }
    }
    return false;
}

int PCx_Tree::deleteTree(unsigned int treeId)
{
    if(treeId==0)
    {
        qFatal("Assertion failed");
    }

    QSqlQuery query;

    if(!query.exec(QString("select count(*) from index_arbres where id='%1'").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }
    if(query.next())
    {
        if(query.value(0).toInt()==0)
        {
            qWarning()<<"Trying to delete an inexistant tree !";
            return -1;
        }
    }

    if(!query.exec(QString("select count(*) from index_audits where id_arbre='%1'").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }
    if(query.next())
    {
        if(query.value(0).toInt()>0)
        {
            return 0;
        }
    }

    if(!query.exec(QString("select count(*) from index_reportings where id_arbre='%1'").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }
    if(query.next())
    {
        if(query.value(0).toInt()>0)
        {
            return 0;
        }
    }


    QSqlDatabase::database().transaction();
    if(!query.exec(QString("delete from index_arbres where id='%1'").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }
    if(query.numRowsAffected()!=1)
    {
        qCritical()<<query.lastError();
        die();
    }


    if(!query.exec(QString("drop table arbre_%1").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }

    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError();
        die();
    }

    if(!query.exec(QString("drop table types_%1").arg(treeId)))
    {
        qCritical()<<query.lastError();
        die();
    }
    if(query.numRowsAffected()==-1)
    {
        qCritical()<<query.lastError();
        die();
    }

    QSqlDatabase::database().commit();

    //qDebug()<<"Tree "<<treeId<<" deleted.";
    return 1;
}



bool PCx_Tree::checkIdToTypeAndName(unsigned int id, const QString &typeName, const QString &nodeName) const
{
    QSqlQuery q;
    QPair<QString,QString> typeAndNodeName=getTypeNameAndNodeName(id);
    if(typeAndNodeName.first!=typeName || typeAndNodeName.second!=nodeName)
        return false;
    return true;
}




int PCx_Tree::importTreeFromXLSX(const QString &fileName, const QString &treeName)
{
    if(fileName.isEmpty() || treeName.isEmpty() || treeNameExists(treeName) || treeName.size()>MAXOBJECTNAMELENGTH)
    {
        qFatal("Assertion failed");
    }
    QFileInfo fi(fileName);
    if(!fi.isReadable()||!fi.isFile())
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Fichier invalide ou non lisible"));
        return -1;
    }

    QXlsx::Document xlsx(fileName);

    if(!xlsx.read(1,1).isValid()||
            !xlsx.read(1,2).isValid()||
            !xlsx.read(1,3).isValid()||
            !xlsx.read(1,4).isValid())
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Format de fichier invalide"));
        return -1;
    }


    int i=2;



    struct _node
    {
        unsigned int id;
        QString typeName;
        QString nodeName;
        unsigned int pid;
    };

    QVector<_node> foundNodes;
    QSet<QString> foundTypes;
    QSet<unsigned int> foundIds;

    int rowCount=xlsx.dimension().rowCount();

    do
    {
        QVariant cellNodeId,cellNodeType,cellNodeName,cellParentId;
        cellNodeId=xlsx.read(i,1);
        cellNodeType=xlsx.read(i,2);
        cellNodeName=xlsx.read(i,3);
        cellParentId=xlsx.read(i,4);

        if(!(cellNodeId.isValid() && cellNodeType.isValid() && cellNodeName.isValid() && cellParentId.isValid()))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Format de fichier invalide ligne %1. Le fichier doit contenir des données sur les quatre premières colonnes").arg(i));
            return -1;
        }

        _node aNode;
        aNode.id=cellNodeId.toUInt();
        aNode.typeName=cellNodeType.toString().simplified();
        aNode.nodeName=cellNodeName.toString().simplified();
        aNode.pid=cellParentId.toUInt();

        if(aNode.id<=1)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Format de fichier invalide ligne %1. L'identifiant du noeud doit être un entier strictement supérieur à 1").arg(i));
            return -1;
        }
        if(aNode.pid<1)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Format de fichier invalide ligne %1. L'identifiant du noeud doit être un entier supérieur ou égal à 1 (1 indique la racine de l'arbre)").arg(i));
            return -1;
        }

        if(aNode.typeName.size()>MAXNODENAMELENGTH)
        {
            QMessageBox::critical(nullptr,QObject::tr("Erreur"),QObject::tr("Type du noeud trop long ligne %1 colonne 2 !").arg(i));
            return -1;
        }

        if(aNode.typeName.isEmpty())
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Type du noeud manquant ou invalide ligne %1 colonne 2 !").arg(i));
            return -1;
        }

        if(aNode.nodeName.size()>MAXNODENAMELENGTH)
        {
            QMessageBox::critical(nullptr,QObject::tr("Erreur"),QObject::tr("Nom du noeud trop long ligne %1 colonne 3 !").arg(i));
            return -1;
        }

        if(aNode.nodeName.isEmpty())
        {
            QMessageBox::critical(nullptr,QObject::tr("Erreur"),QObject::tr("Nom du noeud manquant ou invalide ligne %1 colonne 3 !").arg(i));
            return -1;
        }

        foundTypes.insert(aNode.typeName);
        foundNodes.append(aNode);
        if(foundIds.contains(aNode.id))
        {
            QMessageBox::critical(nullptr,QObject::tr("Erreur"),QObject::tr("Doublon trouvé ligne %1 colonne 1, il existe déjà un noeud ayant cet identifiant !").arg(i));
            return -1;
        }
        foundIds.insert(aNode.id);

        i++;
        if(i>PCx_Tree::MAXNODES)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Trop de noeuds dans l'arbre (maximum %1) !").arg(PCx_Tree::MAXNODES));
            return -1;
        }

    }while(i<=rowCount);

    //Check if all nodes have a valid PID, ie no orphan nodes
    foreach(const _node & aNode,foundNodes)
    {
        if(aNode.pid!=1 && !foundIds.contains(aNode.pid))
        {
            QMessageBox::critical(nullptr,QObject::tr("Erreur"),QObject::tr("Le noeud père (%1) du noeud %2 n'existe pas !").arg(aNode.pid).arg(aNode.id));
            return -1;
        }
    }

    //Check if there is at least one first level node, ie one node has the root for PID
    bool okRoot=false;
    foreach(const _node & aNode,foundNodes)
    {
        if(aNode.pid==1)
        {
            okRoot=true;
            break;
        }
    }
    if(okRoot==false)
    {
        QMessageBox::critical(nullptr,QObject::tr("Erreur"),QObject::tr("Aucun noeud n'est accroché à la racine. Renseignez '1' comme identifiant du père colonne 4 pour indiquer un noeud accroché à la racine."));
        return -1;
    }

    QSqlDatabase::database().transaction();

    int treeId=_internalAddTree(treeName,true);
    if(treeId<=0)
    {
        qFatal("Assertion failed");
    }

    PCx_Tree tree(treeId);

    QHash<QString,unsigned int> typesToIdTypes;

    QStringList foundTypesSorted=foundTypes.toList();
    foundTypesSorted.sort();

    foreach(const QString & oneType,foundTypesSorted)
    {
        unsigned int oneTypeId=tree.addType(oneType);
        if(oneTypeId==0)
        {
            QMessageBox::warning(0,QObject::tr("Erreur"),QObject::tr("Type %1 invalide").arg(oneType.toHtmlEscaped()));
            QSqlDatabase::database().rollback();
            return -1;
        }
        typesToIdTypes.insert(oneType,oneTypeId);
    }

    foreach(const _node & aNode,foundNodes)
    {
        tree.addNode(aNode.pid,typesToIdTypes.value(aNode.typeName),aNode.nodeName,aNode.id);
    }

    QSqlDatabase::database().commit();
    return treeId;













    /*






        if(node1Pid!=1)
        {
            if(!(cellParentType.isValid()||cellParentName.isValid()))
            {
                QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Type ou nom du noeud parent manquants ligne %1 colonne 4 ou 5 !").arg(i));
                return -1;
            }
            if(nodeToPid.contains(node1))
                {
                    if(nodeToPid.value(node1)!=node1Pid)
                    {
                        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Le noeud %1 ne peut pas avoir plusieurs pères !")
                                              .arg(QString(node1.first+" "+node1.second).toHtmlEscaped()));
                        return -1;
                    }
                    else
                    {
                        duplicateFound=true;
                    }
                }
                else
                {
                    nodeToPid.insert(node1,node1Pid);
                }
                foundTypes.insert(pidNode.first);
            }
            else
            {
                if(pidNode.first.size()>MAXNODENAMELENGTH)
                {
                    QMessageBox::critical(nullptr,QObject::tr("Erreur"),QObject::tr("Type du noeud père trop long ligne %1 colonne 3 !").arg(i));
                    return -1;
                }


                if(pidNode.first.isEmpty())
                {
                    QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Type du noeud père manquant ligne %1 colonne 3 !").arg(i));
                    return -1;
                }

                if(pidNode.second.size()>MAXNODENAMELENGTH)
                {
                    QMessageBox::critical(nullptr,QObject::tr("Erreur"),QObject::tr("Nom du noeud père trop long ligne %1 colonne 4 !").arg(i));
                    return -1;
                }


                if(pidNode.second.isEmpty())
                {
                    QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Nom du noeud père manquant ligne %1 colonne 4 !").arg(i));
                    return -1;
                }
            }
        }
        else
        {
            firstLevelNodes.append(node1);
        }
        i++;
        if(i>PCx_Tree::MAXNODES)
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Trop de noeuds dans l'arbre (%1) !").arg(PCx_Tree::MAXNODES));
            return -1;
        }

    }while(i<=rowCount);

    firstLevelNodesSet=firstLevelNodes.toSet();

    if(firstLevelNodesSet.size()<firstLevelNodes.size()||duplicateFound)
    {
        QMessageBox::information(0,QObject::tr("Information"),QObject::tr("Il y a des doublons dans le fichier, ils ne seront pas pris en compte."));
    }


    if(firstLevelNodesSet.size()==0)
    {
        QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Aucun noeud n'est accroché à la racine. Laissez les colonnes 3 et 4 vides pour les spécifier."));
        return -1;
    }


    QPair<QString,QString> firstLevelNode;
    foreach(firstLevelNode,firstLevelNodesSet)
    {
        if(nodeToPid.contains(firstLevelNode))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Le noeud %1 ne peut pas avoir plusieurs pères !")
                                  .arg(QString(firstLevelNode.first+" "+firstLevelNode.second).toHtmlEscaped()));
            return -1;
        }
    }

    QHash<QString,unsigned int> typesToIdTypes;

    i=1;
    QStringList foundTypesList=foundTypes.toList();
    foundTypesList.sort();
    foreach(const QString &oneType, foundTypesList)
    {
        typesToIdTypes.insert(oneType,i);
        i++;
    }

    //Check for orphan ancestors
    QPair<QString, QString> ancestor;

    foreach(ancestor,nodeToPid.values())
    {
        if(!nodeToPid.contains(ancestor) && !firstLevelNodesSet.contains(ancestor))
        {
            QMessageBox::critical(0,QObject::tr("Erreur"),QObject::tr("Le noeud %1 est orphelin !")
                                  .arg(QString(ancestor.first+" "+ancestor.second).toHtmlEscaped()));
            return -1;
        }
    }



    QPair<QString,QString> aNode;
    QMap<QPair<QString,QString>, unsigned int> nodeToIdNode;
    unsigned int k=2;
    foreach(aNode,firstLevelNodesSet)
    {
        nodeToIdNode.insert(aNode,k);
        k++;
    }

    foreach(aNode,nodeToPid.keys())
    {
        nodeToIdNode.insert(aNode,k);
        k++;
    }

    QSqlDatabase::database().transaction();

    int treeId=_internalAddTree(treeName,true);
    if(treeId<=0)
    {
        qFatal("Assertion failed");
    }

    PCx_Tree tree(treeId);

    foreach(const QString & oneType,foundTypesList)
    {
        unsigned int oneTypeId=tree.addType(oneType);
        if(oneTypeId==0)
        {
            QMessageBox::warning(0,QObject::tr("Erreur"),QObject::tr("Type %1 invalide").arg(oneType.toHtmlEscaped()));
            QSqlDatabase::database().rollback();
            return -1;
        }
    }

    foreach(aNode,firstLevelNodesSet)
    {
        tree.addNode(1,typesToIdTypes.value(aNode.first),aNode.second);
    }
    foreach(aNode,nodeToPid.keys())
    {
        QPair<QString, QString> aPid;
        aPid=nodeToPid.value(aNode);
        tree.addNode(nodeToIdNode.value(aPid),typesToIdTypes.value(aNode.first),aNode.second);
    }

    QSqlDatabase::database().commit();
    return treeId;
    */








}
