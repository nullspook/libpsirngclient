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

#include "health.grpc.pb.h"
#include "rng.grpc.pb.h"

#include <fstream>
#include <grpcpp/create_channel.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <sstream>

using namespace google::protobuf;
using namespace grpc;
using namespace grpc::health::v1;
using namespace psirng;

struct psirngclient {
    psirng::Rng::Stub* rng_stub;
    grpc::health::v1::Health::Stub* health_stub;
};

int psirngclient_init(psirngclient** p, const char* host, int grpc_port, const char* cert_path)
{
    std::string target = std::string(host) + ":" + std::to_string(grpc_port);

    if (cert_path && *cert_path != '\0') {
        std::ifstream cert_ifstream(cert_path);
        if (!cert_ifstream.is_open()) {
            return PSIRNGCLIENT_RESULT_ERROR_OPEN_CERTIFICATE;
        }
        std::stringstream cert_stringstream;
        cert_stringstream << cert_ifstream.rdbuf();

        SslCredentialsOptions ssl_opts;
        ssl_opts.pem_root_certs = cert_stringstream.str();

        cert_ifstream.close();

        std::shared_ptr<ChannelCredentials> creds = SslCredentials(ssl_opts);

        ChannelArguments args;

        BIO* bio = BIO_new(BIO_s_mem());
        BIO_puts(bio, ssl_opts.pem_root_certs.c_str());
        X509* x509 = PEM_read_bio_X509(bio, nullptr, nullptr, nullptr);
        BIO_free(bio);
        if (!x509) {
            return PSIRNGCLIENT_RESULT_ERROR_PARSE_CERTIFICATE;
        }
        X509_NAME* x509_name = X509_get_subject_name(x509);
        ASN1_STRING* asn1_string = X509_NAME_ENTRY_get_data(X509_NAME_get_entry(x509_name, X509_NAME_get_index_by_NID(x509_name, OBJ_txt2nid("CN"), -1)));
        std::string cn = std::string((char*)ASN1_STRING_get0_data(asn1_string), ASN1_STRING_length(asn1_string));
        if (cn == "psirng testing") {
            std::cerr << "WARNING: USING TESTING CERTIFICATE. DO NOT USE THIS IN PRODUCTION." << std::endl;
            args.SetSslTargetNameOverride("*");
        }

        std::shared_ptr<Channel> channel = CreateCustomChannel(target, creds, args);

        psirng::Rng::Stub* rng_stub = new psirng::Rng::Stub(channel);
        health::v1::Health::Stub* health_stub = new health::v1::Health::Stub(channel);

        *p = (psirngclient*)malloc(sizeof(psirngclient));
        (*p)->rng_stub = rng_stub;
        (*p)->health_stub = health_stub;

        return PSIRNGCLIENT_RESULT_OK;
    }

    std::shared_ptr<ChannelCredentials> creds = InsecureChannelCredentials();
    std::shared_ptr<Channel> channel = CreateChannel(target, creds);

    psirng::Rng::Stub* rng_stub = new psirng::Rng::Stub(channel);
    health::v1::Health::Stub* health_stub = new health::v1::Health::Stub(channel);

    *p = (psirngclient*)malloc(sizeof(psirngclient));
    (*p)->rng_stub = rng_stub;
    (*p)->health_stub = health_stub;

    return PSIRNGCLIENT_RESULT_OK;
}

void psirngclient_free(psirngclient* p)
{
    if (p) {
        if (p->rng_stub) {
            delete p->rng_stub;
            p->rng_stub = nullptr;
        }
        if (p->health_stub) {
            delete p->health_stub;
            p->health_stub = nullptr;
        }
        free(p);
        p = nullptr;
    }
}

int psirngclient_randbits(psirngclient* p, int* dest, int length)
{
    int buffer_length = (length + 7)  >> 3;
    std::unique_ptr<char[]> buffer(new char[buffer_length]);
    int result = psirngclient_randbytes(p, buffer.get(), buffer_length);
    if (result != PSIRNGCLIENT_RESULT_OK) {
        return result;
    }
    for (int i = 0; i < length; i++) {
        dest[i] = (buffer[i >> 3] >> (i & 7)) & 1;
    }
    return PSIRNGCLIENT_RESULT_OK;
}

int psirngclient_randbytes(psirngclient* p, char* dest, int length)
{
    RandBytesRequest request;
    request.set_length(length);

    RandBytesResponse response;

    ClientContext context;

    Status status = p->rng_stub->RandBytes(&context, request, &response);

    if (status.ok()) {
        memcpy(dest, response.data().data(), length);
        return PSIRNGCLIENT_RESULT_OK;
    } else {
        return PSIRNGCLIENT_RESULT_ERROR_GRPC;
    }
}

int psirngclient_randintegers(psirngclient* p, int* dest, int length, int min, int max)
{
    RandIntegersRequest request;
    request.set_min(min);
    request.set_max(max);
    request.set_length(length);

    RandIntegersResponse response;

    ClientContext context;

    Status status = p->rng_stub->RandIntegers(&context, request, &response);

    if (status.ok()) {
        memcpy(dest, response.data().data(), length * sizeof(int));
        return PSIRNGCLIENT_RESULT_OK;
    } else {
        return PSIRNGCLIENT_RESULT_ERROR_GRPC;
    }
}

int psirngclient_randuniform(psirngclient* p, double* dest, int length, double min, double max)
{
    RandUniformRequest request;
    request.set_length(length);
    request.set_min(min);
    request.set_max(max);

    RandUniformResponse response;

    ClientContext context;

    Status status = p->rng_stub->RandUniform(&context, request, &response);

    if (status.ok()) {
        memcpy(dest, response.data().data(), length * sizeof(double));
        return PSIRNGCLIENT_RESULT_OK;
    } else {
        return PSIRNGCLIENT_RESULT_ERROR_GRPC;
    }
}

int psirngclient_randnormal(psirngclient* p, double* dest, int length, double mean, double stddev)
{
    RandNormalRequest request;
    request.set_length(length);
    request.set_mean(mean);
    request.set_stddev(stddev);

    RandNormalResponse response;

    ClientContext context;

    Status status = p->rng_stub->RandNormal(&context, request, &response);

    if (status.ok()) {
        memcpy(dest, response.data().data(), length * sizeof(double));
        return PSIRNGCLIENT_RESULT_OK;
    } else {
        return PSIRNGCLIENT_RESULT_ERROR_GRPC;
    }
}

bool psirngclient_ishealthy(psirngclient* p)
{
    ClientContext context;

    HealthCheckRequest request;
    request.set_service("rng");

    HealthCheckResponse response;

    Status status = p->health_stub->Check(&context, request, &response);

    if (status.ok()) {
        return response.status() == HealthCheckResponse::SERVING;
    } else {
        return false;
    }
}