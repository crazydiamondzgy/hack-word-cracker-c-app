#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "office.h"

static int 
word_open(CrackContext *ctx)
{
	u8 buf[512];
    int i, section_size;
	u32 x;
	OfficeContext *wc = (OfficeContext *)ctx->priv_data;
	
	ctx->input_fp = fopen(ctx->input_filename, "rb");
	if (NULL == ctx->input_fp)
	{
		printf("failed to open input wordfile!\n");
		exit(-1);
	}
	
	fread(buf, 1, 512, ctx->input_fp);
	
	//
	// 获得每个section的尺寸
	//
	
	x = *(u8 *)(buf+30);
	section_size = (u32)pow(2, x);
	
	//
	// 获得directory的地址
	//
	
	x = *(u8 *)(buf+48);
	x = x*section_size + 512;
	
	//
	// 读取directory的全部数据
	//
	
	rewind(ctx->input_fp);
	fseek(ctx->input_fp, x, SEEK_SET);
	fread(buf, 1, 512, ctx->input_fp);
	
	//
	// 获得1Table的地址
	//
	
	for (i=0; i<section_size; i+=128)
	{
		if (wcscmp(buf+i, L"1Table") == 0)
		{
			wc->table1_pos = *(u32 *)(buf+i+116);
			wc->table1_pos = wc->table1_pos*section_size + 512;
			wc->table1_len = *(u32 *)(buf+i+120);
			continue;
		}
		
		if (wcscmp(buf+i, L"WordDocument") == 0)
		{
			wc->document_pos = *(u32 *)(buf+i+116);
			wc->document_pos = wc->document_pos*section_size + 512;
			wc->document_len = *(u32 *)(buf+i+120);
			continue;
		}
	}
	
	if (0 == wc->table1_pos || 0 == wc->document_pos)
	{
		exit(-1);
	}
	
	//
	// 读取1Table的内容
	//
	
	rewind(ctx->input_fp);
	fseek(ctx->input_fp, wc->table1_pos, SEEK_SET);
	fread(buf, 1, 512, ctx->input_fp);
	
	for (i=0; i<4; i++)
		x = buf[i];
	
    for (i=0; i<16; i++)
		wc->docid[i] = buf[4+i];
	
    for (i=0; i<16; i++)
		wc->salt[i] = buf[20+i];
	
    for (i=0; i<16; i++)
		wc->hashedsalt[i] = buf[36+i];
	
	//
	// 关闭word文件
	//
	
	fclose(ctx->input_fp);

	return 0;
}


int 
remove_word()
{
/*	int j, file_size = 0;
	unsigned char in[64];
	unsigned char tmp[128];
	u8 *file_buf = NULL, *p = NULL;
	DWORD t = GetTickCount();
	FILE *fp;
	unsigned int blk;
	rc4_key key;
	
	//word_open();
	
	//
	// 读word文件
	//
	
	fp = fopen(filepath, "rb");
	if (NULL == fp)
	{
		//		MessageBox("failed to open input wordfile!\n");
		return -1;
	}
	
	//
	// 获得word文件尺寸
	//
	
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	
	file_buf = (u8 *)malloc(file_size);
	if (file_buf == NULL)
	{
		//		MessageBox("failed to alloc the memory for read!\n");
		return -1;
	}
	
	fseek(fp, 0, SEEK_SET);
	fread(file_buf, 1, file_size, fp);
	fclose(fp);
	
	//
	// 解码1Table
	//
	
    blk = 0, j = 0;
	memcpy(in, the_key, 5);
    makekey (blk, &key, in);
	p = file_buf + table1_pos;
	while (j < table1_len)
	{
		rc4 (p, 0x10, &key);
		
		j += 0x10;
		p += 0x10;
		if ((j % 0x200) == 0)
		{
			blk++;
			memcpy(in, the_key, 5);
			makekey (blk, &key, in);
		}
    }
	
	//
	// 解码WordDocument
	//
	
    blk = 0, j = 0;
	memcpy(in, the_key, 5);
    makekey (blk, &key, in);
	
	//
	// 保存前44个字节
	//
	
	p = file_buf + document_pos;
	memcpy(tmp, p, 0x44);
	
	while (j < document_len)
	{
		rc4 (p, 0x10, &key);
		
		j += 0x10;
		p += 0x10;
		if ((j % 0x200) == 0)
		{
			blk++;
			memcpy(in, the_key, 5);
			makekey (blk, &key, in);
		}
    }
	
	p = file_buf + document_pos;
	memcpy(p, tmp, 0x44);
	
	//
	// 取消读密码
	//
	
	*(p + 0x0B) &= ~0x1;
	
	//
	// 取消写密码
	//
	
	*(p + 0x0B) &= ~0x8;
	
	//
	// 取消文档保护密码
	//
	
	dop_pos = *(u32 *)(p + 0x0192);
	dop_len = *(u32 *)(p + 0x0196);
	p = file_buf + table1_pos + dop_pos;
	*(p + 12) &= ~0x10;
	*(p + 7) &= ~0x02;
	*(p + 15) &= ~0x40;
	*(p + 0x4E) = 0;
	*(p + 0x4F) = 0;
	*(p + 0x50) = 0;
	*(p + 0x51) = 0;
	
	//
	// 保存破解后的文件
	//
	
	fp = fopen("foo.doc", "wb");
	if (fp == NULL)
	{
		//		MessageBox("open output word file failed!\n");
		return -1;
	}
	
	fwrite(file_buf, 1, file_size, fp);
	fclose(fp);
*/	
	return 0;
}

Cracker word_cracker = 
{
	"matrix word cracker", 
	"doc", 
	CRACK_TYPE_DICTIONARY | CRACK_TYPE_BRUTEFORCE, 
    CRACK_ID_WORD, 
	sizeof(OfficeContext), 
	word_open, 
	office_crack, 
	office_close, 
	NULL
};
