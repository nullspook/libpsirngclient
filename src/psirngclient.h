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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define PSIRNGCLIENT_RESULT_OK 0
#define PSIRNGCLIENT_RESULT_ERROR_OPEN_CERTIFICATE -92
#define PSIRNGCLIENT_RESULT_ERROR_PARSE_CERTIFICATE -24
#define PSIRNGCLIENT_RESULT_ERROR_GRPC -41

typedef struct psirngclient psirngclient;

int psirngclient_init(psirngclient** p, const char* host, int grpc_port, const char* cert_path);
void psirngclient_free(psirngclient* p);
int psirngclient_randbits(psirngclient* p, int* dest, int length);
int psirngclient_randbytes(psirngclient* p, char* dest, int length);
int psirngclient_randintegers(psirngclient* p, int* dest, int length, int min, int max);
int psirngclient_randuniform(psirngclient* p, double* dest, int length, double min, double max);
int psirngclient_randnormal(psirngclient* p, double* dest, int length, double mean, double stddev);
bool psirngclient_ishealthy(psirngclient* p);

#ifdef __cplusplus
}
#endif