This is a addressbook-like example that shows how to use the KVList, a Qt based library

# Prerequisites 
Qt & cmake

# What's the purpose of KVList

When it comes to presenting data with QML, Qt's model-view architecture is often being used to implement this.
The KVList classes provide a abstraction on top of Qt's model classes that supply a simplification for many common usecases.

# Where can I find the sourcecode for KVList / how can I use it
it can be found [here:](https://github.com/toruki80/KVList/tree/main/src/libs/kvlist)

Using it is as simple as copying the folder into your cmake-based project and adding it as subproject. 
```
...
add_subdirectory(path/to/kvlist)
target_link_libraries(${PROJECT_NAME} lib1 lib2 kvlist)
...
```

# What's it all about?

Let's say we have a 'Addressbook' that holds a number of 'Person' objects. And each 'Person' has a couple of characteristics, such as 'age' and 'name'...
KVList gives you a generic key-value based list, that can be easily exposed to QML. And it reduces writing "boilerplate" code (such as Q_PROPERTY) a lot. And many other advantages I will explain later.. 


Let's start simple.. Here is an example how to implement the above usecase with KVList...

```
// person.h

#include "kvlistentry.h"

class Person : public KVListEntry
{
    Q_OBJECT
public:    
    enum EnKey { name, age, address };
    Q_ENUM(EnKey) // the enum must be registered to Qt's meta object via Q_ENUM

    Person(const QString &name, int age, const QString &address)
    {
        setValue(EnKey::name, name);
        setValue(EnKey::age, age);
        setValue(EnKey::address, address);
    }
};
```
```
// addressbook.h

#include "kvlistmodel.h"
#include "person.h" 

class Addressbook : public KVListModel
{
    Q_OBJECT
public:
    Addressbook() : KVListModel(QMetaEnum::fromType<Person::EnKey>())
    {
        *this << new Person("Adrian", 30, "Munich");
        *this << new Person("Barbara", 40, "Boston");
        *this << new Person("Caesar", 50, "Rome");
    }
};
```

... register the types to QML ...

```
qmlRegisterSingletonType<Addressbook>("KVListExample", 1, 0, "Addressbook", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
    return new Addressbook();
});
```
... and then you can access the data from QML
```
// in qml
import KVListExample 1.0

...
GridView {                                       
    model: Addressbook                           
    delegate: Text {                             
        id: addressbookEntry                     
        text: model.name + ", Age: " + model.age 
    }                                            
}
...
```

There is no need to write these cumbersome Q_PROPERTY getter/setter/notifies any more.
Adding another 'property' is as easy as extending the enum with another value. The type of the property can be anything, as long as QVariant supports it.
Custom types must therefore be added to Qt's metasystem.

# features

### [c++] get/set/notify
```
// person.h

Q_INVOKABLE void incrementAgeDelayed() {
    // wish happy birthay when age changes
    KVListEntry::onValueChanged(EnKey::age, [=](){
        qDebug() << "Happy birthday" << getValue(EnKey::name);
    });

    QTimer::singleShot(500, [=](){
        int oldAge = KVListEntry::getValue(EnKey::age).toInt(); // QValue
        KVListEntry::setValue(EnKey::age, ++oldAge);
    });
}
```
All changes via API e.g. `setValue(KEY, VALUE)` will *automatically* invalidate the parent model. 
E.g. in the above example the UI will show the new age without any additional code. 

### [c++] nice API
here are some examples.. you will get the idea.
```
#main.cpp 

Addressbook *ab = new Addressbook();
Person *p = new Person();

// compact initialization
p->setValues({
  {Person::age, 30},
  {Person::name, "Donald"},
  // ... 
});
         
*ab << p;

// range based loops
for(KVListEntry* e : ab)
  qDebug() << "age:" e->getValue(EnKey::age);

// print all available KEYS
qDebug() << e-getParentModel()->keyNamesList()

// move / remove entries
ab->takeAt(0);
ab->move(1,0);

```


### [qml] tipps & tricks
use model of the current item outside the item:
```
#qml 

ListView {
    id: mylist
    model: Addressbook                           
    delegate: Text {                             
        id: addressbookEntry
        property var myModel: model
        text: model.name + ", Age: " + model.age 
    }                                            
}

// from outside:
Text {
    text: mylist.currentItem.item.myModel.age
}
```

easy access to the actual entry/model for calling functions
```
#qml

delegate: MouseArea {
  onClicked: {
    console.log(model.ENTRY, model.MODEL); //result: 'Person(0x7f855cb22130) Addressbook(0x7f8557f304e0)'
    console.log(model.MODEL.keyNamesList());
    model.ENTRY.incrementAgeDelayed();
  }
}
```

### usecase: edit dialog
A quite common usecase is an "edit-dialog" where the user can change properties. Usually you need to store the changes in case the user cancels the dialog later, or, in case user clicks 'save' apply them.
This usecase is really easy to implement with the KVLIST.
For every KEY exists a shodowed KEY 'KEY_shadowed': e.g. 'age' and 'age_shadowed'.
There is a function to apply/reject the changes which can be called directly form qml. In case you need to validate the new VALUES first, you can override the apply function. 

```
#qml

Text { text: "change age here" }
TextField {
  text: model.age
  onEditionFinished: model.age_shadowed = text
}

Button {
  text: "accept"
  onClicked: model.ENTRY.applyShadowedChanges()
}

Button {
  text: "cancel"
  onClicked: model.ENTRY.revertShadowedChanges()
}
```


### nested models
Another common usecase is to have nested models. 
E.g. Addressbook' contains various 'Person', which again contain a model that contains 'Child'
```

Person.h

class ChildrenModel : public KVListModel { .... };
class ChildrenEntry : public KVListEntry { ... };

class Person : public KVListEntry
{
    Q_OBJECT
public:
    enum EnKey { name, age, address, childrenmodel };
    Q_ENUM(EnKey)

    Person(const QString &name, int age, const QString &address)
    {
        setValue(EnKey::name, name);
        ChildrenModel *cm = new ChildrenModel();
        *cm << new ChildrenEntry() << new ChildrenEntry();
        setChildModel(EnKey::childrenmodel, cm)
    }
};

#qml
# in your listview
ListView {
    id: personlist
    model: Addressbook                           
    delegate: Item{
        ...
        ListView {
            id: childrenlist
            model: parent.model.childrenmodel
            delegate: ....
        }
    }                        
}
```

### (de)serialization 

KVList supports serialization and deserialization without any additional implementation.
Keep in mind that the VALUE must support Qt's `QVariant::toString()`.
Even nested models are being (de)serialized in a recursive fashion.
For now only xml is supported, but e.g. a JSON implementation should not be too hard.

```
#main.cpp
# need to register your type, so it can be constructed during deserialization
REGISTER_2_SERIALIZATION_FACTORY(Person);
REGISTER_2_SERIALIZATION_FACTORY(Adressbook);

qmlRegisterSingletonType<Addressbook>("KVListExample", 1, 0, "Addressbook", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
    Addressbook * ab = new Addressbook();
    ab->setSerializationFile("/app/data/location/addressbook.xml");
    ab->deSerialize();
});


# qml
Button {
  text: "accept"
  onClicked: {
    if(model.ENTRY.applyShadowedChanges() > 0)
        model.MODEL.serialize();
  }
}
```

Sometimes is problematic to (de)serialize certain KEY/VALUES, e.g. you don't want the deserializer to overwrite your existing value... This can be achieved by appending '_noserialize' or '_ns' to the KEY name.
```
class Person : public KVListEntry
{
    Q_OBJECT
public:    
    enum EnKey { name, age, address, usagecount_ns };
    Q_ENUM(EnKey)
};
```

