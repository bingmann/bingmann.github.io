/*******************************************************************************
 * tests/test_alg_game.cpp
 *
 * Test exchange graph generation.
 *
 * Copyright (C) 2013 Timo Bingmann <tb@panthema.net>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "alg_game.h"
#include "bispanning.h"
#include "enumerate.h"

class TestAlgGame
{
public:
    explicit TestAlgGame(const std::vector<const char*>& result)
        : result_(result) { }

    void operator () (const BaseGraph& g)
    {
        graphs_.emplace_back(g);
    }

    ~TestAlgGame()
    {
        ASSERT(graphs_.size() == result_.size() / 2);

        for (size_t i = 0, j = 0; i < graphs_.size(); ++i) {
            std::string base6 = write_graph6(graphs_[i]);
            if (base6 != result_[j]) {
                std::cout << "/* base6 " << i << " */ \"" << base6 << "\",\n";
            }
            ++j;

            AlgGame<BaseGraph> game(graphs_[i]);

            std::string tau6 = write_graph6(game.tau_);
            if (tau6 != result_[j]) {
                std::cout << "/* tau6  " << i << " */ \"" << tau6 << "\",\n";
            }
            ++j;
        }
    }

protected:
    std::vector<BaseGraph> graphs_;

    const std::vector<const char*>& result_;
};

int main()
{
    static const std::vector<const char*> atomic4 = {
        /* base6 0 */
        "C~",
        /* tau6  0 */ "Ks]IIGoK?F_]",
    };

    enumerate_bispanning_graphs("atomic 4", TestAlgGame(atomic4));

    static const std::vector<const char*> all5 = {
        /* base6 0 */
        "DV{",
        /* tau6  0 */ ":W___`AB_?_C`@D`E`EaBaADaBbBDcCFJLdEEMfGGNfHHNgHIKgHIKiIJLjKKLoPQQSToPRRST",
        /* base6 1 */ "D]{",
        /* tau6  1 */ ":[_?___b_D`@A`@C`@aCdFIaEeJbbBJJbCdGIMeEPPfHKfLQQgMNNgHOhKLNNsTVhMOQQWiIKKOOrSTUYY",
    };

    enumerate_bispanning_graphs("5", TestAlgGame(all5));

    static const std::vector<const char*> all6 = {
        /* base6 0 */
        "EC~w",
        /* tau6  0 */ ":o_OGCCCB_?G?A?C__OkCAE_`wG]CB`?_kGCE`?wWKJB@`g_OOLGP?_c[Pa`OoYIFBcgo[MRcA@GchGDAak_UJIqPOglHDaqsgUKFq`Wo^KEBQCoWMGrPgw[SebpxA[NFcY@?aPISaPKeVOGy`SiUJHIx_oYLHYxcqZLhZ@cw[NGZ@cy\\NGZPkw[NgjPky\\Ngjpw}^OG{ADAaPG{q\\OgTIdavKfSiTYlWl",
        /* base6 1 */ "EEzw",
        /* tau6  1 */ ":w_?GCA@B_?G?UA@@OOGQA@__OoYCCaa@cGNA@@gwuFB@bPkKEE``GgcWPa@?sSIRb@ox]MFIdX?g\\GCApW}OOKeXHCcQcaaccQJDdhOw_QHVBiASIDaxPCgXKgX_o[MIDH_o]UebQxlJMFBrD?_WRSQHMcQHcsqYeSIDihSm^QiZ@_qXUJJPoy]QHJPguaRur`pOlUvRh|QiTVbp|CjoGCQ\\OiTWSJ@arpGsYdUlUlKaPIdRhtYn[mVkEJHku",
        /* base6 2 */ "EEnw",
        /* tau6  2 */ ":o_O?CA@@@`G?A?D__OSCAE_`wG]CB@o__QIDCB@kGCEBO_w[\\BAOoWYECa@?sSIHHDGo[MTcA@Gc^GDAaC_UJGQPOglHDaqsgUPHq`XCfKEcQKoWNHCh_seRebPxOhMFDQh[}`fcA@WkagcQPQcRHdIx_oYLGyxcqZLgz@cw[NGJ@cy\\NGJPkw[QHZPky\\QHZpw}`QHJy@?`Qh[q\\OgTIdavKfSiTYlWl",
        /* base6 3 */ "EFzo",
        /* tau6  3 */ ":~?@G_?A?_GS?@G??_?Q@?_II@?OGAWCDAgCEaWCL`o[IBWQA?pAA@_YFCGGF_wKE_wOU_oUHDWSO`OWYaP[[`_s\\`o[WEG_GB?oQa?yGBpQHApmHB@YIA`_capSVHWkMEWo]bQGbGwsYEaaMDp[]F`}NFqSdha_jJGwTEQgiJX?YGx?\\FQAQFA{ocPSTJq}PDqSoKH_pKbMPD@cXc`K^cpw]KHKiJq}RD@kZIQi`JawqMHO^LBiUErQUEPc[gQkrKr[zgACkLi?aIB[wgQGfHrGtgqKcHbo{hA[gNRuhJR|AkRSuNZWxMRk{NSPCns@@OSHBPSV",
        /* base6 4 */ "EFzW",
        /* tau6  4 */ ":w_?GC?@?_PG?YA@@OOGKF@?`WGECA@o_kWOL`@wwyECAOogUKEGoow[fCa@@oyGDbA`hUMFKEH?_QHEQ@W}OOKeXGcSIHQPGkVIIdasgUVdbq@Wl[NgXXSmXKgh_sYQfBaXhHKFbqd?_SQrPg{]VJsQhUaPJDciUaQJdyX_o\\QIY`OqXUJJPku]RUb`pMlUur`|?gSVRx|QiTVRq@?jnGSIXMiTWcQ|arnGsYdUlUlKaPKeSIDYn[nWKEJHku",
        /* base6 5 */ "EQzw",
        /* tau6  5 */ ":o_O?CA@@@`G?A?D__OSCAE_`wG]CB@o__QIDCB@kGCEBO_w[\\BAOoWYECa@?sSIHHDGo[MTcA@Gc^GDAaC_UJGQPOglHDaqsgUPHq`XCfKEcQKoWNHCh_seRebPxOhMFDQh[}`fcA@WkagcQPQcRHdIx_oYLGyxcqZLgz@cw[NGJ@cy\\NGJPkw[QHZPky\\QHZpw}`QHJy@?`Qh[q\\OgTIdavKfSiTYlWl",
        /* base6 6 */ "EQ~o",
        /* tau6  6 */ "~?@GsACAMHaOOK@A@SA_D?@C?E??Q@G?@C??@CBK??@_??A_??C_???OO@_o???????@_??CE???O@_???@_????I??O?A_?C??I?_???g@????K?AG??I??_??E?@A??@O?A???D??_???S?@????W?H????W?G_???B?_o????d??????CW??????IO??????Q_??????GOA?D???CG?_@O???oE????????o??????E_o???????W????????W?????????K??O?????o??WG?????Y_?O?????@`_@???????q_?_??????d_?O??????LG?O??????@i?A????????L@?????????iC?????????oe_?????????E?????????ERO???????????Bg?????????]Y?????????@fg",
        /* base6 7 */ "EUZw",
        /* tau6  7 */ ":{_O?C?@?_PG?YAA@OOGKFE_`WG]MI`?_WMCAA`WOIEBAoo[OGIDP?cQGDbDWoWgSb`r@aMFJdx?cQMFQ@W{^IKEBXqOKGRRHczHCaqKcQKECaSgm[OQpWoWUJR@h?eRebRPhCafB`w{bMFCAS{]OGDat|HOHDA`cqhiDAl[oWOHd\\CcUtIyPSkYLJiXKgSMfTIeeRIdSQIqXLEdap[mlfcIDNZNhSjHd[MFbqLEgSWCADAbPjTmPGdQjtzDbeTkEBDbeRiDAvQmYLuzbesYlUrXox",
        /* base6 8 */ "EUzo",
        /* tau6  8 */ ":~?@C_GAC??A?_?A??wC@@GCD_OWE_gKI_OIA_oyA@?}A@O}BAPEBCW_M`?OD`?WEDGOS`OWECg[Q`pME@g[FBo}F@ooKa?_HDW_JCG_LBPAJCWoRFaEHBOsPa_wMa_gaGggLEwkLEpuKBo{^GQQMB`[ZF@qMCHGSFaiQCpOdHQiTD`WWEAmVEQmTE`o[d`]UEx[XEaokiQgiJrIWEPuWEPkZe`g\\GaIcIRA[F@sgIB]`Gq[tLba]Fa?_GqKnJx{_Hr?pMH{cHQSiIi?eKBE`GqWeLBUdHR?qKbOujQwoLBYlJrCtLZKvMro~mbg{NRw}OCF",
        /* base6 9 */ "EUzW",
        /* tau6  9 */ ":w_?G?A@?_PG?YA@@?gGKF@A@GGSJEBAgG[MK`?_[GCD`?gWUKMGoo{[dCa@APIGDbBqHUMFKEH?_QMFQ@?k]NcAA@AQJGcX`GcYMqPhKfLIDBsgiTdAapW{]dAqx]WWKEriEYLKeSaQ[OHra`crNIdas{_VgCq`WlfSIi@SmXKiyHCeRJdzPo}eRUcATIfRusIHCgSUraLQhmFsAHCiTYtY|_qmGSYTIkUKzhs}^PgtivKfSITiv[nWKUjTku",
        /* base6 10 */ "ETzo",
        /* tau6  10 */ ":o_OGCCCB_?G?A?C_`wGCJ@?`gG]CB`?_kGCE`?wWKJB@`g_OQLGP?_g[Pa`OoYIFBcgo[MRcAp_}OGCdX?_SUcAp_}QHDAaccUJIq`WklKEBQCoWMGrPgw[SebpxA[NFcY@?aPISaPKeZOGy`SiUJHIx[o[NGYx[q\\Ngj@_qXLhZ@gs[NGZHgs\\NgjXow\\MhZpw}^OG{ADAaPG{qXMfSItavOhSidQlWl",
        /* base6 11 */ "ETzg",
        /* tau6  11 */ ":o_O?CA@@@`G?A?D__OSCAE_`wG]CB@o__QIDCB@kGCEBO_w[\\BAOoWYECa@?sSIHHDGo[MTcA@Gc^GDAaC_UJGQPOglHDaqsgUPHq`XCfKEcQKoWNHCh_seRebPxOhMFDQh[}`fcA@WkagcQPQcRHdIx_oYLGyxcqZLgz@cw[NGJ@cy\\NGJPkw[QHZPky\\QHZpw}`QHJy@?`Qh[q\\OgTIdavKfSiTYlWl",
    };

    enumerate_bispanning_graphs("6", TestAlgGame(all6));

    return 0;
}

// forced instantiations
template class AlgGameBase<BaseGraph, false>;
template class AlgGameBase<BaseGraph, true>;

/******************************************************************************/
