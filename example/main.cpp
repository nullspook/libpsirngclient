/*
 * Copyright (C) 2025 NullSpook
 *
 * libpsirngclient is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * libpsirngclient is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with libpsirngclient.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "psirngclient.h"
#include <cstdio>
#include <cstdlib>

int main()
{
    int result;
    psirngclient* p = nullptr;

    if ((result = psirngclient_init(&p, "localhost", 50051, "cert.pem")) != PSIRNGCLIENT_RESULT_OK) {
        printf("Error: %d\n", result);
        return EXIT_FAILURE;
    }

    if (!psirngclient_ishealthy(p)) {
        printf("Health check failed\n");
        return EXIT_FAILURE;
    }

    printf("Get 10 bytes of random data\n");
    char buffer[10];
    if ((result = psirngclient_randbytes(p, buffer, 10)) != PSIRNGCLIENT_RESULT_OK) {
        printf("Error: %d\n", result);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 10; ++i) {
        printf("%02X ", buffer[i] & 0xFF);
    }
    printf("\n");

    psirngclient_free(p);

    return EXIT_SUCCESS;
}
