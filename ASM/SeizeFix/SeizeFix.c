// Reemplaza CanUnitSeize (0x08037B64).
// Solo Marth (id 0x01) puede usar Seize, en cualquier capitulo.
int CanUnitSeize_Fix(void* unit)
{
    unsigned char* charData = *(unsigned char**)unit; // Unit+0x00 = pCharacterData
    return charData[4] == 0x01;                       // CharacterData+0x04 = id
}
