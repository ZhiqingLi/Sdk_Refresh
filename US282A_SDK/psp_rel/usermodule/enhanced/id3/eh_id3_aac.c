#include "eh_id3.h"

/******************************************************************************/
/*
 * \par  Description:解析aac类型的id3信息
 *
 * \param[in]

 * \param[out]

 * \return        TRUE OR FALSE

 * \note
 *******************************************************************************/
bool get_id3_aac(void)
{
    if (FALSE != get_id3_aax())
    {
        return TRUE;
    }

    vfs_file_seek(eh_vfs_mount, 0, SEEK_SET, id3_handle);
    if (FALSE != get_id3_mp3())
    {
        return TRUE;
    }
    vfs_file_seek(eh_vfs_mount, 0, SEEK_SET, id3_handle);
    return get_id3_ape();
}

