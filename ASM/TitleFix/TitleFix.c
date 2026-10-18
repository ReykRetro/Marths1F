// Reemplaza GetChapterTitleWM (0x0808979C).
// Siempre devuelve el campo "Chapter Title" (+0x0E) de la fila del capitulo,
// sin pasar por la rama de nodos con monstruos del mapa mundial.

// GetROMChapterStruct (0x08034618), con bit thumb
static unsigned char* (* const GetChapterRow)(int) =
    (unsigned char* (*)(int))(0x08034618 + 1);

int GetChapterTitleWM_Fix(void* playSt)
{
    if (playSt == 0)
        return 0x54; // "No data", como el original

    // PlaySt+0x0E = chapterIndex
    int chapterIndex = ((signed char*)playSt)[0x0E];

    // ROMChapterData+0x0E = chapTitleId
    return GetChapterRow(chapterIndex)[0x0E];
}
