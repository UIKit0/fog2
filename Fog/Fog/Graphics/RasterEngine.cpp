// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/CpuInfo.h>
#include <Fog/Graphics/RasterEngine_p.h>

namespace Fog {

// ============================================================================
// [Fog::RasterEngine - Demultiply Reciprocal Data]
// ============================================================================

// Reciprocal table created by material provided by M Joonas Pihlaja, see
// <http://cgit.freedesktop.org/~joonas/unpremultiply/tree/README>
//
// Copyright (c) 2009  M Joonas Pihlaja
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

// The reciprocal_table[i] entries are defined by
//
//  0       when i = 0
//  255 / i when i > 0
//
// represented in fixed point format with RECIPROCAL_BITS of
// precision and errors rounded up.
//
// #define ceil_div(a, b) ((a) + (b)-1) / (b)
//
// for each:
//   (i) ? ceil_div(255 * (1 << 16), (i)) : 0) : 0

const uint32_t raster_demultiply_reciprocal_table_d[256] =
{
  0x00000000, 0x00FF0000, 0x007F8000, 0x00550000, 0x003FC000, 0x00330000, 0x002A8000, 0x00246DB7,
  0x001FE000, 0x001C5556, 0x00198000, 0x00172E8C, 0x00154000, 0x00139D8A, 0x001236DC, 0x00110000,
  0x000FF000, 0x000F0000, 0x000E2AAB, 0x000D6BCB, 0x000CC000, 0x000C2493, 0x000B9746, 0x000B1643,
  0x000AA000, 0x000A3334, 0x0009CEC5, 0x000971C8, 0x00091B6E, 0x0008CB09, 0x00088000, 0x000839CF,
  0x0007F800, 0x0007BA2F, 0x00078000, 0x00074925, 0x00071556, 0x0006E454, 0x0006B5E6, 0x000689D9,
  0x00066000, 0x00063832, 0x0006124A, 0x0005EE24, 0x0005CBA3, 0x0005AAAB, 0x00058B22, 0x00056CF0,
  0x00055000, 0x0005343F, 0x0005199A, 0x00050000, 0x0004E763, 0x0004CFB3, 0x0004B8E4, 0x0004A2E9,
  0x00048DB7, 0x00047944, 0x00046585, 0x00045271, 0x00044000, 0x00042E2A, 0x00041CE8, 0x00040C31,
  0x0003FC00, 0x0003EC4F, 0x0003DD18, 0x0003CE55, 0x0003C000, 0x0003B217, 0x0003A493, 0x00039770,
  0x00038AAB, 0x00037E40, 0x0003722A, 0x00036667, 0x00035AF3, 0x00034FCB, 0x000344ED, 0x00033A55,
  0x00033000, 0x000325EE, 0x00031C19, 0x00031282, 0x00030925, 0x00030000, 0x0002F712, 0x0002EE59,
  0x0002E5D2, 0x0002DD7C, 0x0002D556, 0x0002CD5D, 0x0002C591, 0x0002BDF0, 0x0002B678, 0x0002AF29,
  0x0002A800, 0x0002A0FE, 0x00029A20, 0x00029365, 0x00028CCD, 0x00028657, 0x00028000, 0x000279CA,
  0x000273B2, 0x00026DB7, 0x000267DA, 0x00026218, 0x00025C72, 0x000256E7, 0x00025175, 0x00024C1C,
  0x000246DC, 0x000241B3, 0x00023CA2, 0x000237A7, 0x000232C3, 0x00022DF3, 0x00022939, 0x00022493,
  0x00022000, 0x00021B82, 0x00021715, 0x000212BC, 0x00020E74, 0x00020A3E, 0x00020619, 0x00020205,
  0x0001FE00, 0x0001FA0C, 0x0001F628, 0x0001F253, 0x0001EE8C, 0x0001EAD4, 0x0001E72B, 0x0001E38F,
  0x0001E000, 0x0001DC80, 0x0001D90C, 0x0001D5A4, 0x0001D24A, 0x0001CEFB, 0x0001CBB8, 0x0001C881,
  0x0001C556, 0x0001C235, 0x0001BF20, 0x0001BC15, 0x0001B915, 0x0001B61F, 0x0001B334, 0x0001B052,
  0x0001AD7A, 0x0001AAAB, 0x0001A7E6, 0x0001A52A, 0x0001A277, 0x00019FCC, 0x00019D2B, 0x00019A91,
  0x00019800, 0x00019578, 0x000192F7, 0x0001907E, 0x00018E0D, 0x00018BA3, 0x00018941, 0x000186E6,
  0x00018493, 0x00018246, 0x00018000, 0x00017DC2, 0x00017B89, 0x00017958, 0x0001772D, 0x00017508,
  0x000172E9, 0x000170D1, 0x00016EBE, 0x00016CB2, 0x00016AAB, 0x000168AA, 0x000166AF, 0x000164B9,
  0x000162C9, 0x000160DE, 0x00015EF8, 0x00015D18, 0x00015B3C, 0x00015966, 0x00015795, 0x000155C8,
  0x00015400, 0x0001523E, 0x0001507F, 0x00014EC5, 0x00014D10, 0x00014B5F, 0x000149B3, 0x0001480B,
  0x00014667, 0x000144C7, 0x0001432C, 0x00014194, 0x00014000, 0x00013E71, 0x00013CE5, 0x00013B5D,
  0x000139D9, 0x00013859, 0x000136DC, 0x00013563, 0x000133ED, 0x0001327B, 0x0001310C, 0x00012FA1,
  0x00012E39, 0x00012CD5, 0x00012B74, 0x00012A16, 0x000128BB, 0x00012763, 0x0001260E, 0x000124BD,
  0x0001236E, 0x00012223, 0x000120DA, 0x00011F94, 0x00011E51, 0x00011D11, 0x00011BD4, 0x00011A99,
  0x00011962, 0x0001182C, 0x000116FA, 0x000115CA, 0x0001149D, 0x00011372, 0x0001124A, 0x00011124,
  0x00011000, 0x00010EE0, 0x00010DC1, 0x00010CA5, 0x00010B8B, 0x00010A73, 0x0001095E, 0x0001084B,
  0x0001073A, 0x0001062C, 0x0001051F, 0x00010415, 0x0001030D, 0x00010207, 0x00010103, 0x00010000
};

// Example code how demultiply can be used.
// 
// static void FOG_FASTCALL demultiply_argb(uint32_t* dst, uint32_t const * src, sysuint_t w)
// {
//   for (sysuint_t i = w; i; i--, dst++, src++)
//   {
//     uint32_t rgba = src[0];
//     uint32_t a = (rgba >> 24);
//     uint32_t r = (rgba >> 16) & 255;
//     uint32_t g = (rgba >>  8) & 255;
//     uint32_t b = (rgba      ) & 255;
//     uint32_t recip = demultiply_reciprocal_table[a];
//
//     r = ((r * recip)      ) & 0x00FF0000;
//     g = ((g * recip) >>  8) & 0x0000FF00;
//     b = ((b * recip) >> 16) & 0x000000FF;
//
//     dst[0] = r | g | b | (a << 24);
//   }
// }

// Reciprocal table for MMX / SSE2 PRGB32 pixel demultiply. Table can be created
// by this C code:
//
// for (int i = 0; i < 256; i++)
// {
//   if (i == 0)
//   {
//     printf("  0x0000, 0x0000, 0x0000, 0x0000");
//   }
//   else {
//     int a = 255*256/i;
//     printf("  0x%.4X, 0x%.4X, 0x%.4X, 0x%.4X", a, a, a, 256);
//   }
//   printf(",\n");
// }

const uint16_t raster_demultiply_reciprocal_table_w[256 * 4] =
{
  0x0000, 0x0000, 0x0000, 0x0000,
  0xFF00, 0xFF00, 0xFF00, 0x0100,
  0x7F80, 0x7F80, 0x7F80, 0x0100,
  0x5500, 0x5500, 0x5500, 0x0100,
  0x3FC0, 0x3FC0, 0x3FC0, 0x0100,
  0x3300, 0x3300, 0x3300, 0x0100,
  0x2A80, 0x2A80, 0x2A80, 0x0100,
  0x246D, 0x246D, 0x246D, 0x0100,
  0x1FE0, 0x1FE0, 0x1FE0, 0x0100,
  0x1C55, 0x1C55, 0x1C55, 0x0100,
  0x1980, 0x1980, 0x1980, 0x0100,
  0x172E, 0x172E, 0x172E, 0x0100,
  0x1540, 0x1540, 0x1540, 0x0100,
  0x139D, 0x139D, 0x139D, 0x0100,
  0x1236, 0x1236, 0x1236, 0x0100,
  0x1100, 0x1100, 0x1100, 0x0100,
  0x0FF0, 0x0FF0, 0x0FF0, 0x0100,
  0x0F00, 0x0F00, 0x0F00, 0x0100,
  0x0E2A, 0x0E2A, 0x0E2A, 0x0100,
  0x0D6B, 0x0D6B, 0x0D6B, 0x0100,
  0x0CC0, 0x0CC0, 0x0CC0, 0x0100,
  0x0C24, 0x0C24, 0x0C24, 0x0100,
  0x0B97, 0x0B97, 0x0B97, 0x0100,
  0x0B16, 0x0B16, 0x0B16, 0x0100,
  0x0AA0, 0x0AA0, 0x0AA0, 0x0100,
  0x0A33, 0x0A33, 0x0A33, 0x0100,
  0x09CE, 0x09CE, 0x09CE, 0x0100,
  0x0971, 0x0971, 0x0971, 0x0100,
  0x091B, 0x091B, 0x091B, 0x0100,
  0x08CB, 0x08CB, 0x08CB, 0x0100,
  0x0880, 0x0880, 0x0880, 0x0100,
  0x0839, 0x0839, 0x0839, 0x0100,
  0x07F8, 0x07F8, 0x07F8, 0x0100,
  0x07BA, 0x07BA, 0x07BA, 0x0100,
  0x0780, 0x0780, 0x0780, 0x0100,
  0x0749, 0x0749, 0x0749, 0x0100,
  0x0715, 0x0715, 0x0715, 0x0100,
  0x06E4, 0x06E4, 0x06E4, 0x0100,
  0x06B5, 0x06B5, 0x06B5, 0x0100,
  0x0689, 0x0689, 0x0689, 0x0100,
  0x0660, 0x0660, 0x0660, 0x0100,
  0x0638, 0x0638, 0x0638, 0x0100,
  0x0612, 0x0612, 0x0612, 0x0100,
  0x05EE, 0x05EE, 0x05EE, 0x0100,
  0x05CB, 0x05CB, 0x05CB, 0x0100,
  0x05AA, 0x05AA, 0x05AA, 0x0100,
  0x058B, 0x058B, 0x058B, 0x0100,
  0x056C, 0x056C, 0x056C, 0x0100,
  0x0550, 0x0550, 0x0550, 0x0100,
  0x0534, 0x0534, 0x0534, 0x0100,
  0x0519, 0x0519, 0x0519, 0x0100,
  0x0500, 0x0500, 0x0500, 0x0100,
  0x04E7, 0x04E7, 0x04E7, 0x0100,
  0x04CF, 0x04CF, 0x04CF, 0x0100,
  0x04B8, 0x04B8, 0x04B8, 0x0100,
  0x04A2, 0x04A2, 0x04A2, 0x0100,
  0x048D, 0x048D, 0x048D, 0x0100,
  0x0479, 0x0479, 0x0479, 0x0100,
  0x0465, 0x0465, 0x0465, 0x0100,
  0x0452, 0x0452, 0x0452, 0x0100,
  0x0440, 0x0440, 0x0440, 0x0100,
  0x042E, 0x042E, 0x042E, 0x0100,
  0x041C, 0x041C, 0x041C, 0x0100,
  0x040C, 0x040C, 0x040C, 0x0100,
  0x03FC, 0x03FC, 0x03FC, 0x0100,
  0x03EC, 0x03EC, 0x03EC, 0x0100,
  0x03DD, 0x03DD, 0x03DD, 0x0100,
  0x03CE, 0x03CE, 0x03CE, 0x0100,
  0x03C0, 0x03C0, 0x03C0, 0x0100,
  0x03B2, 0x03B2, 0x03B2, 0x0100,
  0x03A4, 0x03A4, 0x03A4, 0x0100,
  0x0397, 0x0397, 0x0397, 0x0100,
  0x038A, 0x038A, 0x038A, 0x0100,
  0x037E, 0x037E, 0x037E, 0x0100,
  0x0372, 0x0372, 0x0372, 0x0100,
  0x0366, 0x0366, 0x0366, 0x0100,
  0x035A, 0x035A, 0x035A, 0x0100,
  0x034F, 0x034F, 0x034F, 0x0100,
  0x0344, 0x0344, 0x0344, 0x0100,
  0x033A, 0x033A, 0x033A, 0x0100,
  0x0330, 0x0330, 0x0330, 0x0100,
  0x0325, 0x0325, 0x0325, 0x0100,
  0x031C, 0x031C, 0x031C, 0x0100,
  0x0312, 0x0312, 0x0312, 0x0100,
  0x0309, 0x0309, 0x0309, 0x0100,
  0x0300, 0x0300, 0x0300, 0x0100,
  0x02F7, 0x02F7, 0x02F7, 0x0100,
  0x02EE, 0x02EE, 0x02EE, 0x0100,
  0x02E5, 0x02E5, 0x02E5, 0x0100,
  0x02DD, 0x02DD, 0x02DD, 0x0100,
  0x02D5, 0x02D5, 0x02D5, 0x0100,
  0x02CD, 0x02CD, 0x02CD, 0x0100,
  0x02C5, 0x02C5, 0x02C5, 0x0100,
  0x02BD, 0x02BD, 0x02BD, 0x0100,
  0x02B6, 0x02B6, 0x02B6, 0x0100,
  0x02AF, 0x02AF, 0x02AF, 0x0100,
  0x02A8, 0x02A8, 0x02A8, 0x0100,
  0x02A0, 0x02A0, 0x02A0, 0x0100,
  0x029A, 0x029A, 0x029A, 0x0100,
  0x0293, 0x0293, 0x0293, 0x0100,
  0x028C, 0x028C, 0x028C, 0x0100,
  0x0286, 0x0286, 0x0286, 0x0100,
  0x0280, 0x0280, 0x0280, 0x0100,
  0x0279, 0x0279, 0x0279, 0x0100,
  0x0273, 0x0273, 0x0273, 0x0100,
  0x026D, 0x026D, 0x026D, 0x0100,
  0x0267, 0x0267, 0x0267, 0x0100,
  0x0262, 0x0262, 0x0262, 0x0100,
  0x025C, 0x025C, 0x025C, 0x0100,
  0x0256, 0x0256, 0x0256, 0x0100,
  0x0251, 0x0251, 0x0251, 0x0100,
  0x024C, 0x024C, 0x024C, 0x0100,
  0x0246, 0x0246, 0x0246, 0x0100,
  0x0241, 0x0241, 0x0241, 0x0100,
  0x023C, 0x023C, 0x023C, 0x0100,
  0x0237, 0x0237, 0x0237, 0x0100,
  0x0232, 0x0232, 0x0232, 0x0100,
  0x022D, 0x022D, 0x022D, 0x0100,
  0x0229, 0x0229, 0x0229, 0x0100,
  0x0224, 0x0224, 0x0224, 0x0100,
  0x0220, 0x0220, 0x0220, 0x0100,
  0x021B, 0x021B, 0x021B, 0x0100,
  0x0217, 0x0217, 0x0217, 0x0100,
  0x0212, 0x0212, 0x0212, 0x0100,
  0x020E, 0x020E, 0x020E, 0x0100,
  0x020A, 0x020A, 0x020A, 0x0100,
  0x0206, 0x0206, 0x0206, 0x0100,
  0x0202, 0x0202, 0x0202, 0x0100,
  0x01FE, 0x01FE, 0x01FE, 0x0100,
  0x01FA, 0x01FA, 0x01FA, 0x0100,
  0x01F6, 0x01F6, 0x01F6, 0x0100,
  0x01F2, 0x01F2, 0x01F2, 0x0100,
  0x01EE, 0x01EE, 0x01EE, 0x0100,
  0x01EA, 0x01EA, 0x01EA, 0x0100,
  0x01E7, 0x01E7, 0x01E7, 0x0100,
  0x01E3, 0x01E3, 0x01E3, 0x0100,
  0x01E0, 0x01E0, 0x01E0, 0x0100,
  0x01DC, 0x01DC, 0x01DC, 0x0100,
  0x01D9, 0x01D9, 0x01D9, 0x0100,
  0x01D5, 0x01D5, 0x01D5, 0x0100,
  0x01D2, 0x01D2, 0x01D2, 0x0100,
  0x01CE, 0x01CE, 0x01CE, 0x0100,
  0x01CB, 0x01CB, 0x01CB, 0x0100,
  0x01C8, 0x01C8, 0x01C8, 0x0100,
  0x01C5, 0x01C5, 0x01C5, 0x0100,
  0x01C2, 0x01C2, 0x01C2, 0x0100,
  0x01BF, 0x01BF, 0x01BF, 0x0100,
  0x01BC, 0x01BC, 0x01BC, 0x0100,
  0x01B9, 0x01B9, 0x01B9, 0x0100,
  0x01B6, 0x01B6, 0x01B6, 0x0100,
  0x01B3, 0x01B3, 0x01B3, 0x0100,
  0x01B0, 0x01B0, 0x01B0, 0x0100,
  0x01AD, 0x01AD, 0x01AD, 0x0100,
  0x01AA, 0x01AA, 0x01AA, 0x0100,
  0x01A7, 0x01A7, 0x01A7, 0x0100,
  0x01A5, 0x01A5, 0x01A5, 0x0100,
  0x01A2, 0x01A2, 0x01A2, 0x0100,
  0x019F, 0x019F, 0x019F, 0x0100,
  0x019D, 0x019D, 0x019D, 0x0100,
  0x019A, 0x019A, 0x019A, 0x0100,
  0x0198, 0x0198, 0x0198, 0x0100,
  0x0195, 0x0195, 0x0195, 0x0100,
  0x0192, 0x0192, 0x0192, 0x0100,
  0x0190, 0x0190, 0x0190, 0x0100,
  0x018E, 0x018E, 0x018E, 0x0100,
  0x018B, 0x018B, 0x018B, 0x0100,
  0x0189, 0x0189, 0x0189, 0x0100,
  0x0186, 0x0186, 0x0186, 0x0100,
  0x0184, 0x0184, 0x0184, 0x0100,
  0x0182, 0x0182, 0x0182, 0x0100,
  0x0180, 0x0180, 0x0180, 0x0100,
  0x017D, 0x017D, 0x017D, 0x0100,
  0x017B, 0x017B, 0x017B, 0x0100,
  0x0179, 0x0179, 0x0179, 0x0100,
  0x0177, 0x0177, 0x0177, 0x0100,
  0x0175, 0x0175, 0x0175, 0x0100,
  0x0172, 0x0172, 0x0172, 0x0100,
  0x0170, 0x0170, 0x0170, 0x0100,
  0x016E, 0x016E, 0x016E, 0x0100,
  0x016C, 0x016C, 0x016C, 0x0100,
  0x016A, 0x016A, 0x016A, 0x0100,
  0x0168, 0x0168, 0x0168, 0x0100,
  0x0166, 0x0166, 0x0166, 0x0100,
  0x0164, 0x0164, 0x0164, 0x0100,
  0x0162, 0x0162, 0x0162, 0x0100,
  0x0160, 0x0160, 0x0160, 0x0100,
  0x015E, 0x015E, 0x015E, 0x0100,
  0x015D, 0x015D, 0x015D, 0x0100,
  0x015B, 0x015B, 0x015B, 0x0100,
  0x0159, 0x0159, 0x0159, 0x0100,
  0x0157, 0x0157, 0x0157, 0x0100,
  0x0155, 0x0155, 0x0155, 0x0100,
  0x0154, 0x0154, 0x0154, 0x0100,
  0x0152, 0x0152, 0x0152, 0x0100,
  0x0150, 0x0150, 0x0150, 0x0100,
  0x014E, 0x014E, 0x014E, 0x0100,
  0x014D, 0x014D, 0x014D, 0x0100,
  0x014B, 0x014B, 0x014B, 0x0100,
  0x0149, 0x0149, 0x0149, 0x0100,
  0x0148, 0x0148, 0x0148, 0x0100,
  0x0146, 0x0146, 0x0146, 0x0100,
  0x0144, 0x0144, 0x0144, 0x0100,
  0x0143, 0x0143, 0x0143, 0x0100,
  0x0141, 0x0141, 0x0141, 0x0100,
  0x0140, 0x0140, 0x0140, 0x0100,
  0x013E, 0x013E, 0x013E, 0x0100,
  0x013C, 0x013C, 0x013C, 0x0100,
  0x013B, 0x013B, 0x013B, 0x0100,
  0x0139, 0x0139, 0x0139, 0x0100,
  0x0138, 0x0138, 0x0138, 0x0100,
  0x0136, 0x0136, 0x0136, 0x0100,
  0x0135, 0x0135, 0x0135, 0x0100,
  0x0133, 0x0133, 0x0133, 0x0100,
  0x0132, 0x0132, 0x0132, 0x0100,
  0x0131, 0x0131, 0x0131, 0x0100,
  0x012F, 0x012F, 0x012F, 0x0100,
  0x012E, 0x012E, 0x012E, 0x0100,
  0x012C, 0x012C, 0x012C, 0x0100,
  0x012B, 0x012B, 0x012B, 0x0100,
  0x012A, 0x012A, 0x012A, 0x0100,
  0x0128, 0x0128, 0x0128, 0x0100,
  0x0127, 0x0127, 0x0127, 0x0100,
  0x0126, 0x0126, 0x0126, 0x0100,
  0x0124, 0x0124, 0x0124, 0x0100,
  0x0123, 0x0123, 0x0123, 0x0100,
  0x0122, 0x0122, 0x0122, 0x0100,
  0x0120, 0x0120, 0x0120, 0x0100,
  0x011F, 0x011F, 0x011F, 0x0100,
  0x011E, 0x011E, 0x011E, 0x0100,
  0x011D, 0x011D, 0x011D, 0x0100,
  0x011B, 0x011B, 0x011B, 0x0100,
  0x011A, 0x011A, 0x011A, 0x0100,
  0x0119, 0x0119, 0x0119, 0x0100,
  0x0118, 0x0118, 0x0118, 0x0100,
  0x0116, 0x0116, 0x0116, 0x0100,
  0x0115, 0x0115, 0x0115, 0x0100,
  0x0114, 0x0114, 0x0114, 0x0100,
  0x0113, 0x0113, 0x0113, 0x0100,
  0x0112, 0x0112, 0x0112, 0x0100,
  0x0111, 0x0111, 0x0111, 0x0100,
  0x0110, 0x0110, 0x0110, 0x0100,
  0x010E, 0x010E, 0x010E, 0x0100,
  0x010D, 0x010D, 0x010D, 0x0100,
  0x010C, 0x010C, 0x010C, 0x0100,
  0x010B, 0x010B, 0x010B, 0x0100,
  0x010A, 0x010A, 0x010A, 0x0100,
  0x0109, 0x0109, 0x0109, 0x0100,
  0x0108, 0x0108, 0x0108, 0x0100,
  0x0107, 0x0107, 0x0107, 0x0100,
  0x0106, 0x0106, 0x0106, 0x0100,
  0x0105, 0x0105, 0x0105, 0x0100,
  0x0104, 0x0104, 0x0104, 0x0100,
  0x0103, 0x0103, 0x0103, 0x0100,
  0x0102, 0x0102, 0x0102, 0x0100,
  0x0101, 0x0101, 0x0101, 0x0100,
  0x0100, 0x0100, 0x0100, 0x0100
};

// Generated using:
//
// for (int a = 0; a < 256; a++)
// {
//   printf("  %0.10gf,\n", (a == 0) ? 0.0 : (double)255/(double)a);
// }
const float raster_demultiply_reciprocal_table_f[256] =
{
  0.0f,
  255.0f,
  127.5f,
  85.0f,
  63.75f,
  51.0f,
  42.5f,
  36.42857143f,
  31.875f,
  28.33333333f,
  25.5f,
  23.18181818f,
  21.25f,
  19.61538462f,
  18.21428571f,
  17.0f,
  15.9375f,
  15.0f,
  14.16666667f,
  13.42105263f,
  12.75f,
  12.14285714f,
  11.59090909f,
  11.08695652f,
  10.625f,
  10.2f,
  9.807692308f,
  9.444444444f,
  9.107142857f,
  8.793103448f,
  8.5f,
  8.225806452f,
  7.96875f,
  7.727272727f,
  7.5f,
  7.285714286f,
  7.083333333f,
  6.891891892f,
  6.710526316f,
  6.538461538f,
  6.375f,
  6.219512195f,
  6.071428571f,
  5.930232558f,
  5.795454545f,
  5.666666667f,
  5.543478261f,
  5.425531915f,
  5.3125f,
  5.204081633f,
  5.1f,
  5.0f,
  4.903846154f,
  4.811320755f,
  4.722222222f,
  4.636363636f,
  4.553571429f,
  4.473684211f,
  4.396551724f,
  4.322033898f,
  4.25f,
  4.180327869f,
  4.112903226f,
  4.047619048f,
  3.984375f,
  3.923076923f,
  3.863636364f,
  3.805970149f,
  3.75f,
  3.695652174f,
  3.642857143f,
  3.591549296f,
  3.541666667f,
  3.493150685f,
  3.445945946f,
  3.4f,
  3.355263158f,
  3.311688312f,
  3.269230769f,
  3.227848101f,
  3.1875f,
  3.148148148f,
  3.109756098f,
  3.072289157f,
  3.035714286f,
  3.0f,
  2.965116279f,
  2.931034483f,
  2.897727273f,
  2.865168539f,
  2.833333333f,
  2.802197802f,
  2.77173913f,
  2.741935484f,
  2.712765957f,
  2.684210526f,
  2.65625f,
  2.628865979f,
  2.602040816f,
  2.575757576f,
  2.55f,
  2.524752475f,
  2.5f,
  2.475728155f,
  2.451923077f,
  2.428571429f,
  2.405660377f,
  2.38317757f,
  2.361111111f,
  2.339449541f,
  2.318181818f,
  2.297297297f,
  2.276785714f,
  2.256637168f,
  2.236842105f,
  2.217391304f,
  2.198275862f,
  2.179487179f,
  2.161016949f,
  2.142857143f,
  2.125f,
  2.107438017f,
  2.090163934f,
  2.073170732f,
  2.056451613f,
  2.04f,
  2.023809524f,
  2.007874016f,
  1.9921875f,
  1.976744186f,
  1.961538462f,
  1.946564885f,
  1.931818182f,
  1.917293233f,
  1.902985075f,
  1.888888889f,
  1.875f,
  1.861313869f,
  1.847826087f,
  1.834532374f,
  1.821428571f,
  1.808510638f,
  1.795774648f,
  1.783216783f,
  1.770833333f,
  1.75862069f,
  1.746575342f,
  1.734693878f,
  1.722972973f,
  1.711409396f,
  1.7f,
  1.688741722f,
  1.677631579f,
  1.666666667f,
  1.655844156f,
  1.64516129f,
  1.634615385f,
  1.624203822f,
  1.613924051f,
  1.603773585f,
  1.59375f,
  1.583850932f,
  1.574074074f,
  1.564417178f,
  1.554878049f,
  1.545454545f,
  1.536144578f,
  1.526946108f,
  1.517857143f,
  1.50887574f,
  1.5f,
  1.49122807f,
  1.48255814f,
  1.473988439f,
  1.465517241f,
  1.457142857f,
  1.448863636f,
  1.440677966f,
  1.43258427f,
  1.424581006f,
  1.416666667f,
  1.408839779f,
  1.401098901f,
  1.393442623f,
  1.385869565f,
  1.378378378f,
  1.370967742f,
  1.363636364f,
  1.356382979f,
  1.349206349f,
  1.342105263f,
  1.335078534f,
  1.328125f,
  1.321243523f,
  1.31443299f,
  1.307692308f,
  1.301020408f,
  1.294416244f,
  1.287878788f,
  1.281407035f,
  1.275f,
  1.268656716f,
  1.262376238f,
  1.256157635f,
  1.25f,
  1.243902439f,
  1.237864078f,
  1.231884058f,
  1.225961538f,
  1.220095694f,
  1.214285714f,
  1.208530806f,
  1.202830189f,
  1.197183099f,
  1.191588785f,
  1.186046512f,
  1.180555556f,
  1.175115207f,
  1.169724771f,
  1.164383562f,
  1.159090909f,
  1.153846154f,
  1.148648649f,
  1.143497758f,
  1.138392857f,
  1.133333333f,
  1.128318584f,
  1.123348018f,
  1.118421053f,
  1.113537118f,
  1.108695652f,
  1.103896104f,
  1.099137931f,
  1.094420601f,
  1.08974359f,
  1.085106383f,
  1.080508475f,
  1.075949367f,
  1.071428571f,
  1.066945607f,
  1.0625f,
  1.058091286f,
  1.053719008f,
  1.049382716f,
  1.045081967f,
  1.040816327f,
  1.036585366f,
  1.032388664f,
  1.028225806f,
  1.024096386f,
  1.02f,
  1.015936255f,
  1.011904762f,
  1.007905138f,
  1.003937008f,
  1.0f
};

// ============================================================================
// [Fog::RasterEngine - Blur Data]
// ============================================================================

const uint16_t raster_linear_blur8_mul[255] =
{
  512, 512, 456, 512, 328, 456, 335, 512, 405, 328, 271, 456, 388, 335, 292, 512,
  454, 405, 364, 328, 298, 271, 496, 456, 420, 388, 360, 335, 312, 292, 273, 512,
  482, 454, 428, 405, 383, 364, 345, 328, 312, 298, 284, 271, 259, 496, 475, 456,
  437, 420, 404, 388, 374, 360, 347, 335, 323, 312, 302, 292, 282, 273, 265, 512,
  497, 482, 468, 454, 441, 428, 417, 405, 394, 383, 373, 364, 354, 345, 337, 328,
  320, 312, 305, 298, 291, 284, 278, 271, 265, 259, 507, 496, 485, 475, 465, 456,
  446, 437, 428, 420, 412, 404, 396, 388, 381, 374, 367, 360, 354, 347, 341, 335,
  329, 323, 318, 312, 307, 302, 297, 292, 287, 282, 278, 273, 269, 265, 261, 512,
  505, 497, 489, 482, 475, 468, 461, 454, 447, 441, 435, 428, 422, 417, 411, 405,
  399, 394, 389, 383, 378, 373, 368, 364, 359, 354, 350, 345, 341, 337, 332, 328,
  324, 320, 316, 312, 309, 305, 301, 298, 294, 291, 287, 284, 281, 278, 274, 271,
  268, 265, 262, 259, 257, 507, 501, 496, 491, 485, 480, 475, 470, 465, 460, 456,
  451, 446, 442, 437, 433, 428, 424, 420, 416, 412, 408, 404, 400, 396, 392, 388,
  385, 381, 377, 374, 370, 367, 363, 360, 357, 354, 350, 347, 344, 341, 338, 335,
  332, 329, 326, 323, 320, 318, 315, 312, 310, 307, 304, 302, 299, 297, 294, 292,
  289, 287, 285, 282, 280, 278, 275, 273, 271, 269, 267, 265, 263, 261, 259
};

const uint8_t raster_linear_blur8_shr[255] =
{
  9 , 11, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17,
  17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
  19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20,
  20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22,
  22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
  22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24
};

// ============================================================================
// [Fog::RasterEngine - RasterFuncs]
// ============================================================================

RasterFuncs rasterFuncs;

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_EXTERN void fog_rasterengine_init_c(void);
#if defined(FOG_ARCH_X86)
FOG_INIT_EXTERN void fog_rasterengine_init_mmx(void);
FOG_INIT_EXTERN void fog_rasterengine_init_mmx3dnow(void);
FOG_INIT_EXTERN void fog_rasterengine_init_mmxsse(void);
#endif // FOG_ARCH_X86
FOG_INIT_EXTERN void fog_rasterengine_init_sse2(void);
FOG_INIT_EXTERN void fog_rasterengine_init_ssse3(void);

FOG_INIT_DECLARE err_t fog_rasterengine_init(void)
{
  using namespace Fog;

  // Install C optimized code (default).
  fog_rasterengine_init_c();

  // Install MMX/MMX3dNow/MMXSSE optimized code if supported.
  //
  // Install it only if x86 architecture is used. In 64-bit mode the SSE2
  // implementation is always available and it's better choice.
#if defined(FOG_ARCH_X86)
  if (cpuInfo->hasFeature(CpuInfo::FEATURE_MMX  )) fog_rasterengine_init_mmx();
  if (cpuInfo->hasFeature(CpuInfo::FEATURE_3dNow)) fog_rasterengine_init_mmx3dnow();
  if (cpuInfo->hasFeature(CpuInfo::FEATURE_SSE  )) fog_rasterengine_init_mmxsse();
#endif // FOG_ARCH_X86

  // Install SSE2 optimized code if supported.
#if defined(FOG_HARDCODE_SSE2)
  fog_rasterengine_init_sse2();
#elif defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
  if (cpuInfo->hasFeature(CpuInfo::FEATURE_SSE2)) fog_rasterengine_init_sse2();
#endif // SSE2

  // Install SSSE3 optimized code if supported.
#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
  if (cpuInfo->hasFeature(CpuInfo::FEATURE_SSSE3)) fog_rasterengine_init_ssse3();
#endif // SSSE3

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_rasterengine_shutdown(void)
{
}
