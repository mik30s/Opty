/*
 * OptionsParser 0.9
 */

#include "OptionsParser.h"

OptionsParser::OptionsParser(){
	init();
}

OptionsParser::OptionsParser(int maxopt)
{
	init( maxopt , maxopt );
}

OptionsParser::OptionsParser(int maxopt, int maxcharopt)
{
	init( maxopt , maxcharopt );
}

OptionsParser::~OptionsParser()
{
	if( mem_allocated )
		cleanup();
}

void
OptionsParser::init()
{
	init( DEFAULT_MAXOPTS , DEFAULT_MAXOPTS );
}

void
OptionsParser::init(int maxopt, int maxcharopt )
{

	max_options 	= maxopt;
	max_char_options = maxcharopt;
	max_usage_lines	= DEFAULT_MAXUSAGE;
	usage_lines	= 0 ;
	argc 		= 0;
	argv 		= NULL;
	posix_style	= true;
	verbose 	= false;
	filename 	= NULL;
	appname 	= NULL;	
	option_counter 	= 0;
	optchar_counter	= 0;
	new_argv 	= NULL;
	new_argc 	= 0 ;
	max_legal_args 	= 0 ;
	command_set 	= false;
	file_set 	= false;
	values 		= NULL;	
	g_value_counter = 0;
	mem_allocated 	= false;
	command_set 	= false;
	file_set	= false;
	opt_prefix_char     = '-';
	file_delimiter_char = ':';
	file_comment_char   = '#';
	equalsign 	= '=';
	comment       = '#' ;
	delimiter     = ':' ;
	endofline     = '\n';
	whitespace    = ' ' ;
	nullterminate = '\0';
	set = false;
	once = true;
	hasoptions = false;
	autousage = false;

	strcpy_s( long_opt_prefix , "--" );

	if( alloc() == false ){
		std::cout << endl << "OPTIONS ERROR : Failed allocating memory" ;
		std::cout << endl ;
		std::cout << "Exiting." << endl ;
		exit (0);
	}
}

bool
OptionsParser::alloc()
{
	int i = 0 ;
	int size = 0 ;

	if( mem_allocated )
		return true;

	size = (max_options+1) * sizeof(const char*);
	options = (const char**)malloc( size );	
	optiontype = (int*) malloc( (max_options+1)*sizeof(int) );	
	optionindex = (int*) malloc( (max_options+1)*sizeof(int) );	
	if( options == NULL || optiontype == NULL || optionindex == NULL )
		return false;
	else
		mem_allocated  = true;
	for( i = 0 ; i < max_options ; i++ ){
		options[i] = NULL;
		optiontype[i] = 0 ;
		optionindex[i] = -1 ;
	}
	optionchars = (char*) malloc( (max_char_options+1)*sizeof(char) );
	optchartype = (int*) malloc( (max_char_options+1)*sizeof(int) );	
	optcharindex = (int*) malloc( (max_char_options+1)*sizeof(int) );	
	if( optionchars == NULL || 
            optchartype == NULL || 
            optcharindex == NULL )
        {
		mem_allocated = false;
		return false;
	}
	for( i = 0 ; i < max_char_options ; i++ ){
		optionchars[i] = '0';
		optchartype[i] = 0 ;
		optcharindex[i] = -1 ;
	}

	size = (max_usage_lines+1) * sizeof(const char*) ;
	usage = (const char**) malloc( size );

	if( usage == NULL  ){
		mem_allocated = false;
		return false;
	}
	for( i = 0 ; i < max_usage_lines ; i++ )
		usage[i] = NULL;

	return true;
}

bool
OptionsParser::doubleOptStorage()
{
	options = (const char**)realloc( options,  
			((2*max_options)+1) * sizeof( const char*) );
	optiontype = (int*) realloc(  optiontype ,  
			((2 * max_options)+1)* sizeof(int) );	
	optionindex = (int*) realloc(  optionindex,  
			((2 * max_options)+1) * sizeof(int) );	
	if( options == NULL || optiontype == NULL || optionindex == NULL )
		return false;
	/* init new storage */
	for( int i = max_options ; i < 2*max_options ; i++ ){
		options[i] = NULL;
		optiontype[i] = 0 ;
		optionindex[i] = -1 ;
	}
	max_options = 2 * max_options ;
	return true;
}

bool
OptionsParser::doubleCharStorage()
{
	optionchars = (char*) realloc( optionchars,  
			((2*max_char_options)+1)*sizeof(char) );
	optchartype = (int*) realloc( optchartype,  
			((2*max_char_options)+1)*sizeof(int) );	
	optcharindex = (int*) realloc( optcharindex,  
			((2*max_char_options)+1)*sizeof(int) );	
	if( optionchars == NULL || 
	    optchartype == NULL || 
	    optcharindex == NULL )
		return false;
	/* init new storage */
	for( int i = max_char_options ; i < 2*max_char_options ; i++ ){
		optionchars[i] = '0';
		optchartype[i] = 0 ;
		optcharindex[i] = -1 ;
	}
	max_char_options = 2 * max_char_options;	
	return true;
}

bool
OptionsParser::doubleUsageStorage()
{
	usage = (const char**)realloc( usage,  
			((2*max_usage_lines)+1) * sizeof( const char*) );
	if ( usage == NULL )
		return false;
	for( int i = max_usage_lines ; i < 2*max_usage_lines ; i++ )
		usage[i] = NULL;
	max_usage_lines = 2 * max_usage_lines ;
	return true;

}


void
OptionsParser::cleanup()
{
	free (options);
	free (optiontype);
	free (optionindex);	
	free (optionchars);
	free (optchartype);
	free (optcharindex);
	free (usage);
        if( values != NULL ) {
            for( int i = 0 ; i < g_value_counter ; i++) {
                free (values[i]);
                values[i] = NULL;
            }
            free (values);
        }
	if( new_argv != NULL )
		free (new_argv);
}

void OptionsParser::setCommandPrefixChar( char _prefix =  )
{
	opt_prefix_char = _prefix;
}

void OptionsParser::setCommandLongPrefix( char *_prefix = "--")
{
	if( strlen( _prefix ) > MAX_LONG_PREFIX_LENGTH ){
		*( _prefix + MAX_LONG_PREFIX_LENGTH ) = '\0'; 
	}

	strcpy_s (long_opt_prefix,  _prefix);
}

void OptionsParser::setFileCommentChar( char _comment = '#' )
{
	file_delimiter_char = _comment;
}


void OptionsParser::setFileDelimiterChar( char _delimiter = ':' )
{
	file_comment_char = _delimiter ;
}

bool OptionsParser::CommandSet() {
	return( command_set );
}

bool OptionsParser::FileSet() {
	return( file_set );
}

void OptionsParser::noPOSIX() {
	posix_style = false;
}

bool OptionsParser::POSIX() {
	return posix_style;
}


void OptionsParser::setVerbose() {
	verbose = true ;
}

void OptionsParser::printVerbose() {
	if( verbose )
		std::cout << '\n'  ;
}
void OptionsParser::printVerbose( const char *msg ) {
	if( verbose )
		std::cout << msg  ;
}

void OptionsParser::printVerbose( char *msg ) {
	if( verbose )
		std::cout << msg  ;
}

void OptionsParser::printVerbose( char ch ) {
	if( verbose )
		std::cout << ch ;
}

bool OptionsParser::hasOptions() {
	return hasoptions;
}

void OptionsParser::autoUsagePrint(bool _autousage) {
	autousage = _autousage;
}

void OptionsParser::useCommandArgs( int _argc, char **_argv ) {
	argc = _argc;
	argv = _argv;
	command_set = true;
	appname = argv[0];
	if(argc > 1) hasoptions = true;
}

void
OptionsParser::useFiileName( const char *_filename )
{
	filename = _filename;
	file_set = true;
}

/*
 * set methods for options 
 */

void
OptionsParser::setCommandOption( const char *opt )
{
	addOption( opt , COMMAND_OPT );
	g_value_counter++;
}

void
OptionsParser::setCommandOption( char opt )
{
	addOption( opt , COMMAND_OPT );
	g_value_counter++;
}

void
OptionsParser::setCommandOption( const char *opt , char optchar )
{
	addOption( opt , COMMAND_OPT );
	addOption( optchar , COMMAND_OPT );
	g_value_counter++;
}

void
OptionsParser::setCommandFlag( const char *opt )
{
	addOption( opt , COMMAND_FLAG );
	g_value_counter++;
}

void
OptionsParser::setCommandFlag( char opt )
{
	addOption( opt , COMMAND_FLAG );
	g_value_counter++;
}

void
OptionsParser::setCommandFlag( const char *opt , char optchar )
{
	addOption( opt , COMMAND_FLAG );
	addOption( optchar , COMMAND_FLAG );
	g_value_counter++;
}

void
OptionsParser::setFileOption( const char *opt )
{
	addOption( opt , FILE_OPT );
	g_value_counter++;
}

void
OptionsParser::setFileOption( char opt )
{
	addOption( opt , FILE_OPT );
	g_value_counter++;
}

void
OptionsParser::setFileOption( const char *opt , char optchar )
{
	addOption( opt , FILE_OPT );
	addOption( optchar, FILE_OPT  );
	g_value_counter++;
}

void
OptionsParser::setFileFlag( const char *opt )
{
	addOption( opt , FILE_FLAG );
	g_value_counter++;
}

void
OptionsParser::setFileFlag( char opt )
{
	addOption( opt , FILE_FLAG );
	g_value_counter++;
}

void
OptionsParser::setFileFlag( const char *opt , char optchar )
{
	addOption( opt , FILE_FLAG );
	addOption( optchar , FILE_FLAG );
	g_value_counter++;
}

void
OptionsParser::setOption( const char *opt )
{
	addOption( opt , COMMON_OPT );
	g_value_counter++;
}

void
OptionsParser::setOption( char opt )
{
	addOption( opt , COMMON_OPT );
	g_value_counter++;
}

void
OptionsParser::setOption( const char *opt , char optchar )
{
	addOption( opt , COMMON_OPT );
	addOption( optchar , COMMON_OPT );
	g_value_counter++;
}

void
OptionsParser::setFlag( const char *opt )
{
	addOption( opt , COMMON_FLAG );
	g_value_counter++;
}

void
OptionsParser::setFlag( const char opt )
{
	addOption( opt , COMMON_FLAG );
	g_value_counter++;
}

void
OptionsParser::setFlag( const char *opt , char optchar )
{
	addOption( opt , COMMON_FLAG );
	addOption( optchar , COMMON_FLAG );
	g_value_counter++;
}

void
OptionsParser::addOption( const char *opt, int type )
{
	if( option_counter >= max_options ){
		if( doubleOptStorage() == false ){
			addOptionError( opt );
			return;
		}
	}
	options[ option_counter ] = opt ;
	optiontype[ option_counter ] =  type ;
	optionindex[ option_counter ] = g_value_counter; 
	option_counter++;
}

void
OptionsParser::addOption( char opt, int type )
{
	if( !POSIX() ){
		printVerbose("Ignoring the option character \"");
		printVerbose(  opt );
		printVerbose( "\" ( POSIX options are turned off )" );
		printVerbose();
		return;
	}


	if( optchar_counter >= max_char_options ){
		if( doubleCharStorage() == false ){
			addOptionError( opt );
			return;
		}
	}
	optionchars[ optchar_counter ] =  opt ;
	optchartype[ optchar_counter ] =  type ;
	optcharindex[ optchar_counter ] = g_value_counter; 
	optchar_counter++;
}

void OptionsParser::addOptionError( const char *opt ) {
	std::cout << endl ;
	std::cout << "OPTIONS ERROR : Failed allocating extra memory " << '\n' ;
	std::cout << "While adding the option : \""<< opt << "\"" << '\n';
	std::cout << "Exiting." << '\n' ;
	std::cout << '\n' ;
}

void OptionsParser::addOptionError( char opt ) {
	std::cout << endl ;
	std::cout << "OPTIONS ERROR : Failed allocating extra memory " << '\n' ;
	std::cout << "While adding the option: \""<< opt << "\"" << '\n';
	std::cout << "Exiting." << '\n' ;
	std::cout << '\n' ;
}

void
OptionsParser::processOptions()
{
	if( ! valueStoreOK() )
		return;
}

void 
OptionsParser::processCommandArgs(int max_args)
{
	max_legal_args = max_args;
	processCommandArgs();
}
 
void
OptionsParser::processCommandArgs( int _argc, char **_argv, int max_args )
{
	max_legal_args = max_args;
	processCommandArgs(  _argc, _argv );
}

void
OptionsParser::processCommandArgs( int _argc, char **_argv )
{
	useCommandArgs( _argc, _argv );
	processCommandArgs();
}

void
OptionsParser::processCommandArgs()
{
   	if( ! ( valueStoreOK() && CommandSet() )  )
	   return;
	   
	if( max_legal_args == 0 )
		max_legal_args = argc;
	new_argv = (int*) malloc( (max_legal_args+1) * sizeof(int) );
	for( int i = 1 ; i < argc ; i++ ){/* ignore first argv */
		if(  argv[i][0] == long_opt_prefix[0] && 
                     argv[i][1] == long_opt_prefix[1] ) { /* long GNU option */
			int match_at = parseGNU( argv[i]+2 ); /* skip -- */
			if( match_at >= 0 && i < argc-1 ) /* found match */
				setValue( options[match_at] , argv[++i] );
		}else if(  argv[i][0] ==  opt_prefix_char ) { /* POSIX char */
			if( POSIX() ){ 
				char ch =  parsePOSIX( argv[i]+1 );/* skip - */ 
				if( ch != '0' && i < argc-1 ) /* matching char */
					setValue( ch ,  argv[++i] );
			} else { /* treat it as GNU option with a - */
				int match_at = parseGNU( argv[i]+1 ); /* skip - */
				if( match_at >= 0 && i < argc-1 ) /* found match */
					setValue( options[match_at] , argv[++i] );
			}
		}else { /* not option but an argument keep index */
			if( new_argc < max_legal_args ){
                                new_argv[ new_argc ] = i ;
                                new_argc++;
                        }else{ /* ignore extra arguments */
                                printVerbose( "Ignoring extra argument: " );
				printVerbose( argv[i] );
				printVerbose( );
				printAutoUsage();
                        }
			printVerbose( "Unknown command argument option : " );
			printVerbose( argv[i] );
			printVerbose( );
			printAutoUsage();
		}
	}
}

char 
OptionsParser::parsePOSIX( char* arg )
{

	for( unsigned int i = 0 ; i < strlen(arg) ; i++ ){ 
		char ch = arg[i] ;
		if( matchChar(ch) ) { /* keep matching flags till an option */
			/*if last char argv[++i] is the value */
			if( i == strlen(arg)-1 ){ 
				return ch;
			}else{/* else the rest of arg is the value */
				i++; /* skip any '=' and ' ' */
				while( arg[i] == whitespace 
			    		|| arg[i] == equalsign )
					i++;	
				setValue( ch , arg+i );
				return '0';
			}
		}
	}
	printVerbose( "Unknown command argument option : " );
	printVerbose( arg );
	printVerbose( );
	printAutoUsage();
	return '0';
}

int
OptionsParser::parseGNU( char *arg )
{
	size_t split_at = 0;
	/* if has a '=' sign get value */
	for( size_t i = 0 ; i < strlen(arg) ; i++ ){
		if(arg[i] ==  equalsign ){
			split_at = i ; /* store index */
			i = strlen(arg); /* get out of loop */
		}
	}
	if( split_at > 0 ){ /* it is an option value pair */
		char* tmp = (char*) malloc(  (split_at+1)*sizeof(char) );
		for( size_t i = 0 ; i < split_at ; i++ )
			tmp[i] = arg[i];
		tmp[split_at] = '\0';

		if ( matchOpt( tmp ) >= 0 ){
			setValue( options[matchOpt(tmp)] , arg+split_at+1 );
			free (tmp);
		}else{
			printVerbose( "Unknown command argument option : " );
			printVerbose( arg );
			printVerbose( );
			printAutoUsage();
			free (tmp);
			return -1;
		}
	}else{ /* regular options with no '=' sign  */
		return  matchOpt(arg);
	}
	return -1;
}


int 
OptionsParser::matchOpt( char *opt )
{
	for( int i = 0 ; i < option_counter ; i++ ){
		if( strcmp( options[i], opt ) == 0 ){
			if( optiontype[i] ==  COMMON_OPT ||
			    optiontype[i] ==  COMMAND_OPT )	
			{ /* found option return index */
				return i;
			}else if( optiontype[i] == COMMON_FLAG ||
			       optiontype[i] == COMMAND_FLAG )
			{ /* found flag, set it */ 
				setFlagOn( opt );
				return -1;
			}
		}
	}
	printVerbose( "Unknown command argument option : " );
	printVerbose( opt  ) ;
	printVerbose( );
	printAutoUsage();
	return  -1;	
}
bool
OptionsParser::matchChar( char c )
{
	for( int i = 0 ; i < optchar_counter ; i++ ){
		if( optionchars[i] == c ) { /* found match */
			if(optchartype[i] == COMMON_OPT ||
			     optchartype[i] == COMMAND_OPT )
			{ /* an option store and stop scanning */
				return true;	
			}else if( optchartype[i] == COMMON_FLAG || 
				  optchartype[i] == COMMAND_FLAG ) { /* a flag store and keep scanning */
				setFlagOn( c );
				return false;
			}
		}
	}
	printVerbose( "Unknown command argument option : " );
	printVerbose( c ) ;
	printVerbose( );
	printAutoUsage();
	return false;
}

bool
OptionsParser::valueStoreOK( )
{
	int size= 0;
	if( !set ){
		if( g_value_counter > 0 ){
			size = g_value_counter * sizeof(char*);
			values = (char**)malloc( size );	
			for( int i = 0 ; i < g_value_counter ; i++)
				values[i] = NULL;
			set = true;
		}
	}
	return  set;
}

/*
 * public get methods 
 */
char*
OptionsParser::getValue( const char *option )
{
	if( !valueStoreOK() )
		return NULL;

	for( int i = 0 ; i < option_counter ; i++ ){
		if( strcmp( options[i], option ) == 0 )
			return values[ optionindex[i] ];
	}
	return NULL;
}

bool
OptionsParser::getFlag( const char *option )
{
	if( !valueStoreOK() )
		return false;
	for( int i = 0 ; i < option_counter ; i++ ){
		if( strcmp( options[i], option ) == 0 )
			return findFlag( values[ optionindex[i] ] );
	}
	return false;
}

char*
OptionsParser::getValue( char option )
{
	if( !valueStoreOK() )
		return NULL;
	for( int i = 0 ; i < optchar_counter ; i++ ){
		if( optionchars[i] == option )
			return values[ optcharindex[i] ];
	}
	return NULL;
}

bool
OptionsParser::getFlag( char option )
{
	if( !valueStoreOK() )
		return false;
	for( int i = 0 ; i < optchar_counter ; i++ ){
		if( optionchars[i] == option )
			return findFlag( values[ optcharindex[i] ] ) ;
	}
	return false;
}

bool
OptionsParser::findFlag( char* val )
{
	if( val == NULL )
		return false;

	if( strcmp( TRUE_FLAG , val ) == 0 )
		return true;

	return false;
}

/*
 * private set methods 
 */
bool
OptionsParser::setValue( const char *option , char *value )
{
	if( !valueStoreOK() )
		return false;
        for( int i = 0 ; i < option_counter ; i++ ){
                if( strcmp( options[i], option ) == 0 ){
                        size_t length = (strlen(value)+1)*sizeof(char);
                        values[ optionindex[i] ] = (char*) malloc(length);
                        strcpy_s( values[ optionindex[i] ], length, value );
			return true;
		}
        }
        return false;
}

bool
OptionsParser::setFlagOn( const char *option )
{
	if( !valueStoreOK() )
		return false;
        for( int i = 0 ; i < option_counter ; i++ ){
                if( strcmp( options[i], option ) == 0 ){
                        size_t length = (strlen(TRUE_FLAG)+1)*sizeof(char);
                        values[ optionindex[i] ] = (char*) malloc(length);
                        strcpy_s( values[ optionindex[i] ], length, TRUE_FLAG );
			return true;
		}
        }
        return false;
}

bool
OptionsParser::setValue( char option , char *value )
{
	if( !valueStoreOK() )
		return false;
        for( int i = 0 ; i < optchar_counter ; i++ ){
                if( optionchars[i] == option ){
                        size_t length = (strlen(value)+1)*sizeof(char);
                        values[ optcharindex[i] ] = (char*) malloc(length);
                        strcpy_s( values[ optcharindex[i] ], length, value );
			return true;
		}
        }
        return false;
}

bool
OptionsParser::setFlagOn( char option )
{
	if( !valueStoreOK() )
		return false;
        for( int i = 0 ; i < optchar_counter ; i++ ){
                if( optionchars[i] == option ){
                        size_t length = (strlen(TRUE_FLAG)+1)*sizeof(char);
                        values[ optcharindex[i] ] = (char*) malloc(length);
			strcpy_s( values[ optcharindex[i] ] , length, TRUE_FLAG );
			return true;
		}
        }
        return false;
}


int
OptionsParser::getArgc( )
{
	return new_argc;
}

char* 
OptionsParser::getArgv( int index )
{
	if( index < new_argc ){
		return ( argv[ new_argv[ index ] ] );
	}
	return NULL;
}

/* dotfile sub routines */

bool
OptionsParser::processFile()
{
	if( ! (valueStoreOK() && FileSet())  )
		return false;
	return  ( consumeFile(readFile()) );
}

bool
OptionsParser::processFile( const char *filename )
{
	useFiileName(filename );
	return ( processFile() );
}

char*
OptionsParser::readFile()
{
	return ( readFile(filename) );
}

/*
 * read the file contents to a character buffer 
 */

char*
OptionsParser::readFile( const char* fname )
{
        char *buffer;
        ifstream is;
        is.open ( fname , ifstream::in );
        if( ! is.good() ){
                is.close();
                return NULL;
        }
        is.seekg (0, ios::end);
        size_t length = (size_t)is.tellg();
        is.seekg (0, ios::beg);
        buffer = (char*) malloc(length*sizeof(char));
        is.read (buffer,length);
        is.close();
        return buffer;
}

/*
 * scans a char* buffer for lines that does not 
 * start with the specified comment character.
 */
bool
OptionsParser::consumeFile( char *buffer )
{

        if( buffer == NULL ) 
		return false;

       	char *cursor = buffer;/* preserve the ptr */
       	char *pline = NULL ;
       	int linelength = 0;
       	bool newline = true;
       	for( unsigned int i = 0 ; i < strlen( buffer ) ; i++ ){
       	if( *cursor == endofline ) { /* end of line */
          	if( pline != NULL ) /* valid line */
               		processLine( pline, linelength );
                 	pline = NULL;
                 	newline = true;
           	}else if( newline ){ /* start of line */
                 	newline = false;
              		if( (*cursor != comment ) ){ /* not a comment */
		    		pline = cursor ;
                    		linelength = 0 ;
                	}
             	}
            	cursor++; /* keep moving */
            	linelength++;
       	}
     	free (buffer);
	return true;
}


/*
 *  find a valid type value pair separated by a delimiter 
 *  character and pass it to valuePairs()
 *  any line which is not valid will be considered a value
 *  and will get passed on to justValue()
 *
 *  assuming delimiter is ':' the behaviour will be,
 *
 *  width:10    - valid pair valuePairs( width, 10 );
 *  width : 10  - valid pair valuepairs( width, 10 );
 *
 *  ::::        - not valid 
 *  width       - not valid
 *  :10         - not valid 
 *  width:      - not valid  
 *  ::          - not valid 
 *  :           - not valid 
 *  
 */

void
OptionsParser::processLine( char *theline, int length  )
{
        bool found = false;
        char *pline = (char*) malloc( (length+1)*sizeof(char) );
        for( int i = 0 ; i < length ; i ++ )
                pline[i]= *(theline++);
        pline[length] = nullterminate;
        char *cursor = pline ; /* preserve the ptr */
        if( *cursor == delimiter || *(cursor+length-1) == delimiter ){
                justValue( pline );/* line with start/end delimiter */
        }else{
                for( int i = 1 ; i < length-1 && !found ; i++){/* delimiter */
                        if( *cursor == delimiter ){
                                *(cursor-1) = nullterminate; /* two strings */
                                found = true;
                                valuePairs( pline , cursor+1 );
                        }
                        cursor++;
                }
                cursor++;
                if( !found ) /* not a pair */
                        justValue( pline );
        }
        free (pline);
}

/*
 * removes trailing and preceeding whitespaces from a string
 */
char*
OptionsParser::chomp( char *str )
{
        while( *str == whitespace )
                str++;
        char *end = str+strlen(str)-1;
        while( *end == whitespace )
                end--;
        *(end+1) = nullterminate;
        return str;
}

void
OptionsParser::valuePairs( char *type, char *value )
{
	if ( strlen(chomp(type)) == 1  ){ /* this is a char option */
		for( int i = 0 ; i < optchar_counter ; i++ ){
			if(  optionchars[i] == type[0]  ){ /* match */
				if( optchartype[i] == COMMON_OPT ||
				    optchartype[i] == FILE_OPT )
				{
			 		setValue( type[0] , chomp(value) );
			 		return;
				}
			}
		}
	}	
	/* if no char options matched */
	for( int i = 0 ; i < option_counter ; i++ ){
		if( strcmp( options[i], type ) == 0 ){ /* match */
			if( optiontype[i] == COMMON_OPT ||
			    optiontype[i] == FILE_OPT )
			{
		 		setValue( type , chomp(value) );
		 		return;
			}
		}
	}
        printVerbose( "Unknown option in resourcefile : " );
	printVerbose( type );
	printVerbose( );
}

void
OptionsParser::justValue( char *type )
{

	if ( strlen(chomp(type)) == 1  ){ /* this is a char option */
		for( int i = 0 ; i < optchar_counter ; i++ ){
			if(  optionchars[i] == type[0]  ){ /* match */
				if( optchartype[i] == COMMON_FLAG ||
				    optchartype[i] == FILE_FLAG )
				{
			 		setFlagOn( type[0] );
			 		return;
				}
			}
		}
	}	
	/* if no char options matched */
	for( int i = 0 ; i < option_counter ; i++ ){
		if( strcmp( options[i], type ) == 0 ){ /* match */
			if( optiontype[i] == COMMON_FLAG ||
			    optiontype[i] == FILE_FLAG )
			{
		 		setFlagOn( type );
		 		return;
			}
		}
	}
        printVerbose( "Unknown option in resourcefile : " );
	printVerbose( type  );
	printVerbose( );
}

/*
 * usage and help 
 */


void
OptionsParser::printAutoUsage()
{
	if( autousage ) printUsage();
}

void
OptionsParser::printUsage()
{
	
	if( once ) {
		once = false ;
		std::cout << endl ;
		for( int i = 0 ; i < usage_lines ; i++ )
			std::cout << usage[i] << endl ;	
		std::cout << endl ;
	}
}
	
	
void
OptionsParser::addUsage( const char *line )
{
	if( usage_lines >= max_usage_lines ){
		if( doubleUsageStorage() == false ){
			addUsageError( line );
			exit(1);
		}
	}
	usage[ usage_lines ] = line ;	
	usage_lines++;
}

void
OptionsParser::addUsageError( const char *line )
{
	std::cout << endl ;
	std::cout << "OPTIONS ERROR : Failed allocating extra memory " << endl ;
	std::cout << "While adding the usage/help  : \""<< line << "\"" << endl;
	std::cout << "Exiting." << endl ;
	std::cout << endl ;
	exit(0);

}
