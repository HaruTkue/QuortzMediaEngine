void Initialize(const char* appName , ISurfaceProvider* surfacePrivder){
    m_surfaceProvider = surfacePrivder;
    CreateInstance(appName);
    PickPhysicalDevice();
    
}