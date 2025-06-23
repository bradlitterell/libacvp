/** @file */
/*****************************************************************************
* Copyright (c) 2024, Cisco Systems, Inc.
* All rights reserved.

* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "ut_common.h"
#include "app_common.h"
#include "iut_common.h"
#include "acvp/acvp_lcl.h"

static ACVP_TEST_CASE *test_case;
static ACVP_PBKDF_TC *pbkdf_tc;
static ACVP_RESULT rv;

int initialize_pbkdf_tc(ACVP_PBKDF_TC *stc,
                        unsigned int tc_id,
                        ACVP_HASH_ALG hmacAlg,
                        ACVP_PBKDF_TESTTYPE testType,
                        const char *salt,
                        const char *password,
                        int iterationCount,
                        int key_len,
                        int salt_len,
                        int password_len,
                        int corrupt) {
    ACVP_RESULT rv;
    int tmp;
    int slen;
    int plen;
    
    if (salt) {
       slen = strlen(salt);
    }
    if (password) {
       plen = strlen(password);
    }
    memzero_s(stc, sizeof(ACVP_PBKDF_TC));
    
    stc->cipher = ACVP_PBKDF;
    stc->hmac_type = hmacAlg;
    stc->test_type = testType;
    stc->key_len = key_len;


    //copy password (string) to TC
    if (stc->password && password) {
        stc->password = calloc(plen + 1, sizeof(char));
        if (!stc->password) { return ACVP_MALLOC_FAIL; }
        tmp = strncpy_s(stc->password, plen + 1, password, plen);
        if (tmp) { return ACVP_DATA_TOO_LARGE; }
    }
    stc->pw_len = password_len;

    stc->iterationCount = iterationCount;

    if (salt) {
        stc->salt = calloc(slen, sizeof(char));
        if (!stc->salt) { return 0; }
        rv = acvp_hexstr_to_bin(salt, stc->salt, ACVP_PBKDF_SALT_LEN_STR_MAX, (int*)&slen);
        if (rv != ACVP_SUCCESS) {
            printf("Hex conversion failure (init_salt)\n");
            return 0;
        }
    }
    stc->salt_len = salt_len;

    
    //Allocate space for output (key)
    if (!corrupt) {
        stc->key = calloc(ACVP_PBKDF_KEY_BYTE_MAX + 1, sizeof(unsigned char));
        if (!stc->key) { return ACVP_MALLOC_FAIL; }
    }
    return 1;
}

void free_pbkdf_tc(ACVP_PBKDF_TC *stc) {
    if (stc->salt) free(stc->salt);
    if (stc->password) free(stc->password);
    if (stc->key) free(stc->key);
    free(stc);
    stc = NULL;
}

/*
 * missing test case
 */
Test(APP_PBKDF_HANDLER, missing_test_case) {
    pbkdf_tc = calloc(1, sizeof(ACVP_PBKDF_TC));

    if (!initialize_pbkdf_tc(pbkdf_tc,
                             1,
                             ACVP_SHA1,
                             ACVP_PBKDF_TEST_TYPE_AFT,
                             "EC3A906D0FFF1103867EBA92F21BE954EBB84EE6608A2268C4FD0A7DD60766AD50C60B2301B39A9338C9887918404560A0DCBC052FA2D3F75E9221A99249CD0FF7D3D2387C8A30A8BE7E89338280FBE5A69DC7A3165D8023FF87AE315B5811C1FD0AFA1325ECFE687FD363B0C6D27F0CC47C01A40BCB1AF1E6F8247AF2B07B212F4F06018DBF61A741D36F304E671C2302A0E2D6794010040C87D287943F3BEF03B118E2F681ACA6527AE51CCF2678B980B8BA2E9F7ED34732FD823965B42226EF3E",
                             "HUjvwLdXyBriTrbBGSnlzdvJlEVnDdcjyuWzCZXAvLkREWOhzNFfAjREzzjSQQuvyiKyvBZGUNQOSDfpQNOQxWJpDsZcQeJEqPtvmpMXO",
                             75,
                             67,
                             194,
                             105, 0)) {

        cr_assert_fail("pbkdf init tc failure");
    }

    test_case = calloc(1, sizeof(ACVP_TEST_CASE));
    test_case->tc.pbkdf = pbkdf_tc;
    
    rv = app_pbkdf_handler(NULL);
    cr_assert_neq(rv, 0);

    free_pbkdf_tc(pbkdf_tc);
    free(test_case);
}


/*
 * missing pbkdf_tc
 */
Test(APP_PBKDF_HANDLER, missing_pbkdf_tc) {
    pbkdf_tc = calloc(1, sizeof(ACVP_PBKDF_TC));

    if (!initialize_pbkdf_tc(pbkdf_tc,
                             1,
                             ACVP_SHA1,
                             ACVP_PBKDF_TEST_TYPE_AFT,
                             "EC3A906D0FFF1103867EBA92F21BE954EBB84EE6608A2268C4FD0A7DD60766AD50C60B2301B39A9338C9887918404560A0DCBC052FA2D3F75E9221A99249CD0FF7D3D2387C8A30A8BE7E89338280FBE5A69DC7A3165D8023FF87AE315B5811C1FD0AFA1325ECFE687FD363B0C6D27F0CC47C01A40BCB1AF1E6F8247AF2B07B212F4F06018DBF61A741D36F304E671C2302A0E2D6794010040C87D287943F3BEF03B118E2F681ACA6527AE51CCF2678B980B8BA2E9F7ED34732FD823965B42226EF3E",
                             "HUjvwLdXyBriTrbBGSnlzdvJlEVnDdcjyuWzCZXAvLkREWOhzNFfAjREzzjSQQuvyiKyvBZGUNQOSDfpQNOQxWJpDsZcQeJEqPtvmpMXO",
                             75,
                             67,
                             194,
                             105, 0)) {

        cr_assert_fail("pbkdf init tc failure");
    }

    test_case = calloc(1, sizeof(ACVP_TEST_CASE));
    test_case->tc.pbkdf = NULL;
    
    rv = app_pbkdf_handler(test_case);
    cr_assert_neq(rv, 0);

    free_pbkdf_tc(pbkdf_tc);
    free(test_case);
}


/*
 * missing hmacAlg in pbkdf tc test case
 */
Test(APP_PBKDF_HANDLER, missing_hmacAlg) {

    pbkdf_tc = calloc(1, sizeof(ACVP_PBKDF_TC));

    if (!initialize_pbkdf_tc(pbkdf_tc,
                             1,
                             0,
                             ACVP_PBKDF_TEST_TYPE_AFT,
                             "EC3A906D0FFF1103867EBA92F21BE954EBB84EE6608A2268C4FD0A7DD60766AD50C60B2301B39A9338C9887918404560A0DCBC052FA2D3F75E9221A99249CD0FF7D3D2387C8A30A8BE7E89338280FBE5A69DC7A3165D8023FF87AE315B5811C1FD0AFA1325ECFE687FD363B0C6D27F0CC47C01A40BCB1AF1E6F8247AF2B07B212F4F06018DBF61A741D36F304E671C2302A0E2D6794010040C87D287943F3BEF03B118E2F681ACA6527AE51CCF2678B980B8BA2E9F7ED34732FD823965B42226EF3E",
                             "HUjvwLdXyBriTrbBGSnlzdvJlEVnDdcjyuWzCZXAvLkREWOhzNFfAjREzzjSQQuvyiKyvBZGUNQOSDfpQNOQxWJpDsZcQeJEqPtvmpMXO",
                             75,
                             67,
                             194,
                             105, 0)) {

        cr_assert_fail("pbkdf init tc failure");
    }

    test_case = calloc(1, sizeof(ACVP_TEST_CASE));
    test_case->tc.pbkdf = pbkdf_tc;
    
    rv = app_pbkdf_handler(test_case);
    cr_assert_neq(rv, 0);

    free_pbkdf_tc(pbkdf_tc);
    free(test_case);
}

/*
 * missing salt in pbkdf tc test case
 */
Test(APP_PBKDF_HANDLER, missing_salt) {
    pbkdf_tc = calloc(1, sizeof(ACVP_PBKDF_TC));

    if (!initialize_pbkdf_tc(pbkdf_tc,
                             1,
                             ACVP_SHA1,
                             ACVP_PBKDF_TEST_TYPE_AFT,
                             NULL,
                             "HUjvwLdXyBriTrbBGSnlzdvJlEVnDdcjyuWzCZXAvLkREWOhzNFfAjREzzjSQQuvyiKyvBZGUNQOSDfpQNOQxWJpDsZcQeJEqPtvmpMXO",
                             75,
                             67,
                             194,
                             105, 0)) {

        cr_assert_fail("pbkdf init tc failure");
    }

    test_case = calloc(1, sizeof(ACVP_TEST_CASE));
    test_case->tc.pbkdf = pbkdf_tc;
    
    rv = app_pbkdf_handler(test_case);
    cr_assert_neq(rv, 0);

    free_pbkdf_tc(pbkdf_tc);
    free(test_case);
}

/*
 * missing salt length in pbkdf tc test case
 */
Test(APP_PBKDF_HANDLER, missing_saltlen) {
    pbkdf_tc = calloc(1, sizeof(ACVP_PBKDF_TC));

    if (!initialize_pbkdf_tc(pbkdf_tc,
                             1,
                             ACVP_SHA1,
                             ACVP_PBKDF_TEST_TYPE_AFT,
                             "EC3A906D0FFF1103867EBA92F21BE954EBB84EE6608A2268C4FD0A7DD60766AD50C60B2301B39A9338C9887918404560A0DCBC052FA2D3F75E9221A99249CD0FF7D3D2387C8A30A8BE7E89338280FBE5A69DC7A3165D8023FF87AE315B5811C1FD0AFA1325ECFE687FD363B0C6D27F0CC47C01A40BCB1AF1E6F8247AF2B07B212F4F06018DBF61A741D36F304E671C2302A0E2D6794010040C87D287943F3BEF03B118E2F681ACA6527AE51CCF2678B980B8BA2E9F7ED34732FD823965B42226EF3E",
                             "HUjvwLdXyBriTrbBGSnlzdvJlEVnDdcjyuWzCZXAvLkREWOhzNFfAjREzzjSQQuvyiKyvBZGUNQOSDfpQNOQxWJpDsZcQeJEqPtvmpMXO",
                             75,
                             67,
                             0,
                             105, 0)) {

        cr_assert_fail("pbkdf init tc failure");
    }

    test_case = calloc(1, sizeof(ACVP_TEST_CASE));
    test_case->tc.pbkdf = pbkdf_tc;
    
    rv = app_pbkdf_handler(test_case);
    cr_assert_neq(rv, 0);

    free_pbkdf_tc(pbkdf_tc);
    free(test_case);
}

/*
 * missing password in pbkdf tc test case
 */
Test(APP_PBKDF_HANDLER, missing_password) {
    pbkdf_tc = calloc(1, sizeof(ACVP_PBKDF_TC));

    if (!initialize_pbkdf_tc(pbkdf_tc,
                             1,
                             ACVP_SHA1,
                             ACVP_PBKDF_TEST_TYPE_AFT,
                             "EC3A906D0FFF1103867EBA92F21BE954EBB84EE6608A2268C4FD0A7DD60766AD50C60B2301B39A9338C9887918404560A0DCBC052FA2D3F75E9221A99249CD0FF7D3D2387C8A30A8BE7E89338280FBE5A69DC7A3165D8023FF87AE315B5811C1FD0AFA1325ECFE687FD363B0C6D27F0CC47C01A40BCB1AF1E6F8247AF2B07B212F4F06018DBF61A741D36F304E671C2302A0E2D6794010040C87D287943F3BEF03B118E2F681ACA6527AE51CCF2678B980B8BA2E9F7ED34732FD823965B42226EF3E",
                             NULL,
                             75,
                             67,
                             194,
                             105, 0)) {

        cr_assert_fail("pbkdf init tc failure");
    }

    test_case = calloc(1, sizeof(ACVP_TEST_CASE));
    test_case->tc.pbkdf = pbkdf_tc;
    
    rv = app_pbkdf_handler(test_case);
    cr_assert_neq(rv, 0);

    free_pbkdf_tc(pbkdf_tc);
    free(test_case);
}

/*
 * missing pw_len in pbkdf tc test case
 */
Test(APP_PBKDF_HANDLER, missing_pwlen) {
    pbkdf_tc = calloc(1, sizeof(ACVP_PBKDF_TC));

    if (!initialize_pbkdf_tc(pbkdf_tc,
                             1,
                             ACVP_SHA1,
                             ACVP_PBKDF_TEST_TYPE_AFT,
                             "EC3A906D0FFF1103867EBA92F21BE954EBB84EE6608A2268C4FD0A7DD60766AD50C60B2301B39A9338C9887918404560A0DCBC052FA2D3F75E9221A99249CD0FF7D3D2387C8A30A8BE7E89338280FBE5A69DC7A3165D8023FF87AE315B5811C1FD0AFA1325ECFE687FD363B0C6D27F0CC47C01A40BCB1AF1E6F8247AF2B07B212F4F06018DBF61A741D36F304E671C2302A0E2D6794010040C87D287943F3BEF03B118E2F681ACA6527AE51CCF2678B980B8BA2E9F7ED34732FD823965B42226EF3E",
                             "HUjvwLdXyBriTrbBGSnlzdvJlEVnDdcjyuWzCZXAvLkREWOhzNFfAjREzzjSQQuvyiKyvBZGUNQOSDfpQNOQxWJpDsZcQeJEqPtvmpMXO",
                             75,
                             67,
                             194,
                             0, 0)) {

        cr_assert_fail("pbkdf init tc failure");
    }

    test_case = calloc(1, sizeof(ACVP_TEST_CASE));
    test_case->tc.pbkdf = pbkdf_tc;
    
    rv = app_pbkdf_handler(test_case);
    cr_assert_neq(rv, 0);

    free_pbkdf_tc(pbkdf_tc);
    free(test_case);
}

/*
 * missing iteration in pbkdf tc test case
 */
Test(APP_PBKDF_HANDLER, missing_iteration) {
    pbkdf_tc = calloc(1, sizeof(ACVP_PBKDF_TC));

    if (!initialize_pbkdf_tc(pbkdf_tc,
                             1,
                             ACVP_SHA1,
                             ACVP_PBKDF_TEST_TYPE_AFT,
                             "EC3A906D0FFF1103867EBA92F21BE954EBB84EE6608A2268C4FD0A7DD60766AD50C60B2301B39A9338C9887918404560A0DCBC052FA2D3F75E9221A99249CD0FF7D3D2387C8A30A8BE7E89338280FBE5A69DC7A3165D8023FF87AE315B5811C1FD0AFA1325ECFE687FD363B0C6D27F0CC47C01A40BCB1AF1E6F8247AF2B07B212F4F06018DBF61A741D36F304E671C2302A0E2D6794010040C87D287943F3BEF03B118E2F681ACA6527AE51CCF2678B980B8BA2E9F7ED34732FD823965B42226EF3E",
                             "HUjvwLdXyBriTrbBGSnlzdvJlEVnDdcjyuWzCZXAvLkREWOhzNFfAjREzzjSQQuvyiKyvBZGUNQOSDfpQNOQxWJpDsZcQeJEqPtvmpMXO",
                             0,
                             67,
                             194,
                             105, 0)) {

        cr_assert_fail("pbkdf init tc failure");
    }

    test_case = calloc(1, sizeof(ACVP_TEST_CASE));
    test_case->tc.pbkdf = pbkdf_tc;
    
    rv = app_pbkdf_handler(test_case);
    cr_assert_neq(rv, 0);

    free_pbkdf_tc(pbkdf_tc);
    free(test_case);
}

/*
 * missing key output allocation in pbkdf tc test case
 */
Test(APP_PBKDF_HANDLER, missing_key) {
    pbkdf_tc = calloc(1, sizeof(ACVP_PBKDF_TC));

    if (!initialize_pbkdf_tc(pbkdf_tc,
                             1,
                             ACVP_SHA1,
                             ACVP_PBKDF_TEST_TYPE_AFT,
                             "EC3A906D0FFF1103867EBA92F21BE954EBB84EE6608A2268C4FD0A7DD60766AD50C60B2301B39A9338C9887918404560A0DCBC052FA2D3F75E9221A99249CD0FF7D3D2387C8A30A8BE7E89338280FBE5A69DC7A3165D8023FF87AE315B5811C1FD0AFA1325ECFE687FD363B0C6D27F0CC47C01A40BCB1AF1E6F8247AF2B07B212F4F06018DBF61A741D36F304E671C2302A0E2D6794010040C87D287943F3BEF03B118E2F681ACA6527AE51CCF2678B980B8BA2E9F7ED34732FD823965B42226EF3E",
                             "HUjvwLdXyBriTrbBGSnlzdvJlEVnDdcjyuWzCZXAvLkREWOhzNFfAjREzzjSQQuvyiKyvBZGUNQOSDfpQNOQxWJpDsZcQeJEqPtvmpMXO",
                             75,
                             67,
                             194,
                             105, 1)) {

        cr_assert_fail("pbkdf init tc failure");
    }

    test_case = calloc(1, sizeof(ACVP_TEST_CASE));
    test_case->tc.pbkdf = pbkdf_tc;
    
    rv = app_pbkdf_handler(test_case);
    cr_assert_neq(rv, 0);

    free_pbkdf_tc(pbkdf_tc);
    free(test_case);
}

/*
 * missing key_len in pbkdf tc test case
 */
Test(APP_PBKDF_HANDLER, missing_keylen) {
    pbkdf_tc = calloc(1, sizeof(ACVP_PBKDF_TC));

    if (!initialize_pbkdf_tc(pbkdf_tc,
                             1,
                             ACVP_SHA1,
                             ACVP_PBKDF_TEST_TYPE_AFT,
                             "EC3A906D0FFF1103867EBA92F21BE954EBB84EE6608A2268C4FD0A7DD60766AD50C60B2301B39A9338C9887918404560A0DCBC052FA2D3F75E9221A99249CD0FF7D3D2387C8A30A8BE7E89338280FBE5A69DC7A3165D8023FF87AE315B5811C1FD0AFA1325ECFE687FD363B0C6D27F0CC47C01A40BCB1AF1E6F8247AF2B07B212F4F06018DBF61A741D36F304E671C2302A0E2D6794010040C87D287943F3BEF03B118E2F681ACA6527AE51CCF2678B980B8BA2E9F7ED34732FD823965B42226EF3E",
                             "HUjvwLdXyBriTrbBGSnlzdvJlEVnDdcjyuWzCZXAvLkREWOhzNFfAjREzzjSQQuvyiKyvBZGUNQOSDfpQNOQxWJpDsZcQeJEqPtvmpMXO",
                             75,
                             0,
                             194,
                             105, 0)) {

        cr_assert_fail("pbkdf init tc failure");
    }

    test_case = calloc(1, sizeof(ACVP_TEST_CASE));
    test_case->tc.pbkdf = pbkdf_tc;
    
    rv = app_pbkdf_handler(test_case);
    cr_assert_neq(rv, 0);

    free_pbkdf_tc(pbkdf_tc);
    free(test_case);
}
