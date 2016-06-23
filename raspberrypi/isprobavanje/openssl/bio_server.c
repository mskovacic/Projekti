 #include <openssl/bio.h>
 
 //Here is a simple server example. 
 //It makes use of a buffering BIO to allow lines to be read from the SSL BIO using BIO_gets. 
 //It creates a pseudo web page containing the actual request from a client and also echoes the request to standard output.
 
 int main(int argc, char **argv) { 
	
	BIO *sbio, *bbio, *acpt, *out;
	int len;
	char tmpbuf[1024];
	SSL_CTX *ctx;
	SSL *ssl;

	ERR_load_crypto_strings();
	ERR_load_SSL_strings();
	OpenSSL_add_all_algorithms();

	/* Might seed PRNG here */

	ctx = SSL_CTX_new(SSLv23_server_method());

	if (!SSL_CTX_use_certificate_file(ctx,"server.pem",SSL_FILETYPE_PEM)
	|| !SSL_CTX_use_PrivateKey_file(ctx,"server.pem",SSL_FILETYPE_PEM)
	|| !SSL_CTX_check_private_key(ctx)) {

	fprintf(stderr, "Error setting up SSL_CTX\n");
	ERR_print_errors_fp(stderr);
	return 0;
	}

	/* Might do other things here like setting verify locations and
	* DH and/or RSA temporary key callbacks
	*/

	/* New SSL BIO setup as server */
	sbio=BIO_new_ssl(ctx,0);

	BIO_get_ssl(sbio, &ssl);

	if(!ssl) {
	fprintf(stderr, "Can't locate SSL pointer\n");
	/* whatever ... */
	}

	/* Don't want any retries */
	SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

	/* Create the buffering BIO */

	bbio = BIO_new(BIO_f_buffer());

	/* Add to chain */
	sbio = BIO_push(bbio, sbio);

	acpt=BIO_new_accept("4433");

	/* By doing this when a new connection is established
	* we automatically have sbio inserted into it. The
	* BIO chain is now 'swallowed' by the accept BIO and
	* will be freed when the accept BIO is freed.
	*/

	BIO_set_accept_bios(acpt,sbio);

	out = BIO_new_fp(stdout, BIO_NOCLOSE);

	/* Setup accept BIO */
	if(BIO_do_accept(acpt) <= 0) {
	fprintf(stderr, "Error setting up accept BIO\n");
	ERR_print_errors_fp(stderr);
	return 0;
	}

	/* Now wait for incoming connection */
	if(BIO_do_accept(acpt) <= 0) {
	fprintf(stderr, "Error in connection\n");
	ERR_print_errors_fp(stderr);
	return 0;
	}

	/* We only want one connection so remove and free
	* accept BIO
	*/

	sbio = BIO_pop(acpt);

	BIO_free_all(acpt);

	if(BIO_do_handshake(sbio) <= 0) {
	fprintf(stderr, "Error in SSL handshake\n");
	ERR_print_errors_fp(stderr);
	return 0;
	}

	BIO_puts(sbio, "HTTP/1.0 200 OK\r\nContent-type: text/plain\r\n\r\n");
	BIO_puts(sbio, "\r\nConnection Established\r\nRequest headers:\r\n");
	BIO_puts(sbio, "--------------------------------------------------\r\n");

	for(;;) {
	len = BIO_gets(sbio, tmpbuf, 1024);
	if(len <= 0) break;
	BIO_write(sbio, tmpbuf, len);
	BIO_write(out, tmpbuf, len);
	/* Look for blank line signifying end of headers*/
	if((tmpbuf[0] == '\r') || (tmpbuf[0] == '\n')) break;
	}

	BIO_puts(sbio, "--------------------------------------------------\r\n");
	BIO_puts(sbio, "\r\n");

	/* Since there is a buffering BIO present we had better flush it */
	BIO_flush(sbio);

	BIO_free_all(sbio);
	return 0;
 }