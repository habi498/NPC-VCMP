class WeaponData
{
public:
    float fRange;
    uint16_t wFiringrate;
    uint16_t wReloadTimeMs;
    uint16_t byteClipsize;
    uint8_t byteDamage;
    WeaponData(float range, uint16_t firingrate, uint16_t reload, uint16_t clipsize, uint8_t damage)
    {
        this->fRange = range;
        this->wFiringrate = firingrate;
        this->wReloadTimeMs = reload;
        this->byteClipsize = clipsize;
        this->byteDamage = damage;
    }
    WeaponData() {}
};
void LoadWeaponData();