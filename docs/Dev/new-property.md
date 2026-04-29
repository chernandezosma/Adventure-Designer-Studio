
# Dev Procedures

1. Add new Property to entity
We

## Add new Property to entity

Here's the exact sequence, using SceneData / Entities::Scene as the example. Say you want to add an ambientSound string property.
                                                                                                                                                               
### Step 1 — Add the field to SceneData.h

Add the private member and its getter/setter:

```c++
// in the public section:                                                                                                                                 
[[nodiscard]] const std::string& getAmbientSound() const { return m_ambientSound; }                                                                         
void setAmbientSound(const std::string& path) { m_ambientSound = path; }

// in the private section:                                                                                                                                  
std::string m_ambientSound; ///< Path to the ambient sound asset
```

### Step 2 — Register it in Scene::getPropertyDescriptors() (Scene.cpp)

This tells the inspector that the property exists, what type it is, and how to display it:

```c++
PropertyDescriptor("ambientSound", "Ambient Sound", PropertyType::String)                                                                                 
.setCategory("Appearance")                                                                                                                              
.setDescription("Path to the ambient sound asset"),
```

### Step 3 — Handle the read in Scene::getPropertyValue() (Scene.cpp)

```c++
if (propertyId == "ambientSound") return m_data->getAmbientSound();
```

### Step 4 — Handle the write in Scene::setPropertyValue() (Scene.cpp)

```c++
else if (propertyId == "ambientSound") {
    if (auto* str = std::get_if<std::string>(&value)) {                                                                                                     
        setAmbientSound(*str);                                                                                                                              
        return true;
    }                                                                                                                                                       
}
```

### Step 5 — Add the typed getter/setter to Scene.cpp (and declare them in Scene.h)

```c++
// Scene.h declaration:
const std::string& getAmbientSound() const;                                                                                                                 
void setAmbientSound(const std::string& path);

// Scene.cpp implementation:
const std::string& Scene::getAmbientSound() const {                                                                                                         
return m_data->getAmbientSound();                                                                                                                     
}

void Scene::setAmbientSound(const std::string& path) {                                                                                                      
    if (m_data->getAmbientSound() != path) {                                                                                                              
        std::string oldPath = m_data->getAmbientSound();                                                                                                    
        m_data->setAmbientSound(path);
        notifyPropertyChanged("ambientSound", oldPath, m_data->getAmbientSound());                                                                          
    }                                                                                                                                                       
}
```                