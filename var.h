#ifndef WIZVAR_H_INCLUDED
#define WIZVAR_H_INCLUDED


#include <wiz/global.h>
#include <wiz/wizardError.h>
#include <sstream>
#include <string>

namespace wiz {
    enum VarType { TINT, TDOUBLE, TSTRING, TBOOL, TOBJECT, TNOTDEFINE }; // "T is Type

    class wizVarError : public wiz::Error { }; // : Error #include "wizardError.h"
    class Var
    {
    private:
        void* var; // void*
        enum VarType type; // type
    private:
        // P : Pointer and inline!!
        int* convertIntP( void* x )const { return static_cast<int*>( x ); }
        double* convertDoubleP( void* x )const { return static_cast<double*>( x ); }
        std::string* convertStringP( void* x )const { return static_cast<std::string*>( x ); }
        wiz::wizObject* convertObjectP( void* x )const { return static_cast<wiz::wizObject*>( x ); }
        bool* convertBoolP( void* x )const { return static_cast<bool*>( x ); }
        void* convertVoidP( int* x )const { return static_cast<void*>( x ); }
        void* convertVoidP( double* x )const { return static_cast<void*>( x ); }
        void* convertVoidP( std::string* x )const { return static_cast<void*>( x ); }
        void* convertVoidP( bool* x )const { return static_cast<void*>( x ); }
        void* convertVoidP( wiz::wizObject* x )const { return static_cast<void*>( x ); } // wizObject <- SmartPtr<wizobject>
    public:
        Var()
        : var(NULL), type(TNOTDEFINE)
        {
        }
        Var( const Var& _var )
        {
            this->type = _var.type;
            switch( _var.type )
            {
            case TINT:
                var = convertVoidP( new int( *convertIntP( _var.var ) ) ); break;
            case TDOUBLE:
                var = convertVoidP( new double( *convertDoubleP( _var.var ) ) ); break;
            case TSTRING:
                var = convertVoidP( new std::string( *convertStringP( _var.var ) ) ); break;
            case TBOOL:
                var = convertVoidP( new bool( *convertBoolP( _var.var ) ) ); break;
            case TOBJECT:
                var = convertVoidP( convertObjectP( _var.var )->clone() ); break; /// chk!
            default:
                throw wizVarError();
            }
        }
        Var& operator=( const Var& _var )
        {
			if (this == &_var) { return *this; } /// chk!!

			if (TNOTDEFINE == type && TNOTDEFINE == _var.type )
			{
				return *this;
			}
			else if (TNOTDEFINE == type)
			{
				this->type = _var.type;
				switch (_var.type)
				{
				case TINT:
					var = convertVoidP(new int(*convertIntP(_var.var))); break;
				case TDOUBLE:
					var = convertVoidP(new double(*convertDoubleP(_var.var))); break;
				case TSTRING:
					var = convertVoidP(new std::string(*convertStringP(_var.var))); break;
				case TBOOL:
					var = convertVoidP(new bool(*convertBoolP(_var.var))); break;
				case TOBJECT:
					var = convertVoidP(convertObjectP(_var.var)->clone()); break; /// chk!
				}

			}

            if( type != _var.type )
            {
                throw wizVarError(); // error check..
            }

            switch( _var.type )
            {
            case TINT:
                set( *convertIntP( _var.var ) ); break;
            case TDOUBLE:
                set( *convertDoubleP( _var.var ) ); break;
            case TSTRING:
                set( *convertStringP( _var.var ) ); break;
            case TBOOL:
                set( *convertBoolP( _var.var ) ); break;
            case TOBJECT:
                set( *convertObjectP( _var.var ) ); break;
            default:
                throw wizVarError();
            }

            return *this;
        }
    public:
        Var( int x );
        Var( double x );
        Var( std::string& str );
        Var( const char* str );
        Var( bool x );
        Var( wizObject& object );

        virtual ~Var();

        // get, set  type이 맞지 않으면 에러!!
        // void*인 경우, try~catch를 사용한다!!
        bool get( void* data )const;
        bool get( int& x )const;
        bool get( double& x )const;
        bool get( std::string& x )const;
        bool get( bool& x )const;
        bool get( wizObject& x )const;

        // set()
        bool set( void* data );
        bool set( int x );
        bool set( double x );
        bool set( const char* x ) { return set( std::string( x ) ); }
        bool set( std::string x );
        bool set( bool x );
        bool set( wizObject& object );

        // getType
        enum VarType getVarType()const { return type; }

        // toString
        std::string toString()const;

		// operator type
		explicit operator wizObject*() {
			if (TOBJECT == type) {
				return static_cast<wizObject*>(var);
			}
			return NULL; // nullptr
		}	
		/// todo ?
    };
    
    
    Var::Var( int x )
    {
        var = convertVoidP( new int );
        *convertIntP( var ) = x;
        type = TINT;
    }
    Var::Var( double x )
    {
        var = convertVoidP( new double );
        *convertDoubleP( var ) = x;
        type = TDOUBLE;
    }
    Var::Var( const char* str )
    {
       var = convertVoidP( new std::string );
       *convertStringP( var ) = str;
       type = TSTRING;
    }
    Var::Var( std::string& x )
    {
        var = convertVoidP( new std::string );
        *convertStringP( var ) = x;
        type = TSTRING;
    }
    Var::Var( bool x )
    {
        var = convertVoidP( new bool );
        *convertBoolP( var ) = x;
        type = TBOOL;
    }
    Var::Var( wizObject& x )
    {
        var = convertVoidP( x.clone() );
        type = TOBJECT;
    }

    Var::~Var()
    {
        switch( type )
        {
        case TINT:
            delete convertIntP( var ); break;
        case TDOUBLE:
            delete convertDoubleP( var ); break;
        case TSTRING:
            delete convertStringP( var ); break;
        case TBOOL:
            delete convertBoolP( var ); break;
        case TOBJECT:
            delete convertObjectP( var ); break;
        case TNOTDEFINE:
            break;
		//default:
            // throw wizVarError(); // err
        }
    }

    bool Var::get( void* data )const
    {
        bool bSuccess = true;
        try
        {
            switch( type )
            {
            case TINT:
                bSuccess = get( *convertIntP( data ) ); break;
            case TDOUBLE:
                bSuccess = get( *convertDoubleP( data ) ); break;
            case TSTRING:
                bSuccess = get( *convertStringP( data ) ); break;
            case TBOOL:
                bSuccess = get( *convertBoolP( data ) ); break;
            case TOBJECT:
                bSuccess = get( *convertObjectP( data ) ); break; // check err.
                break;
            default: // TNOTDEFINE!
                bSuccess = false; //
            }
        }
        catch( std::exception e )
        {
            bSuccess = false;
        }
        return bSuccess;
    }
    bool Var::get( int& x )const
    {
        bool bSuccess = false;
        if( TINT == type )
        {
            x = *convertIntP( var );
            bSuccess = true;
        }
        return bSuccess;
    }
    bool Var::get( double& x )const
    {
        bool bSuccess = false;

        if( TDOUBLE == type )
        {
            x = *convertDoubleP( var );
            bSuccess = true;
        }

        return bSuccess;
    }
    bool Var::get( std::string& x )const
    {
        bool bSuccess = false;

        if( TSTRING == type )
        {
            x = *convertStringP( var );
            bSuccess = true;
        }

        return bSuccess;
    }
    bool Var::get( bool& x )const
    {
        bool bSuccess = false;

        if( TBOOL == type )
        {
            x = *convertBoolP( var );
            bSuccess = true;
        }

        return bSuccess;
    }
    bool Var::get( wiz::wizObject& x )const
    {
        bool bSuccess = false;

        if( TOBJECT == type )
        {
           x = *convertObjectP( var );
            bSuccess = true;
        }

        return bSuccess;
    }

    // set()
    bool Var::set( void* data )
    {
        bool bSuccess = true;
        try
        {
            switch( type )
            {
            case TINT:
                bSuccess = set( *convertIntP( data ) ); break;
            case TDOUBLE:
                bSuccess = set( *convertDoubleP( data ) ); break;
            case TSTRING:
                bSuccess = set( *convertStringP( data ) ); break;
            case TBOOL:
                bSuccess = set( *convertBoolP( data ) ); break;
            case TOBJECT:
                bSuccess = set( *convertObjectP( data ) ); break;
            default:
                bSuccess = false; //
            }
        }
        catch( std::exception e )
        {
            bSuccess = false;
        }
        return bSuccess;
    }
    bool Var::set( int x )
    {
        bool bSuccess = false;
        if( TNOTDEFINE == type )
        {
            var = new int;
            type = TINT;
        }
        if( TINT == type )
        {
            *convertIntP( var ) = x;
            bSuccess = true;
        }
        return bSuccess;
    }
    bool Var::set( double x )
    {
        bool bSuccess = false;
        if( TNOTDEFINE == type )
        {
            var = new double;
            type = TDOUBLE;
        }
        if( TDOUBLE == type )
        {
            *convertDoubleP( var ) = x;
            bSuccess = true;
        }
        return bSuccess;
    }
    bool Var::set( std::string x )
    {
        bool bSuccess = false;
        if( TNOTDEFINE == type )
        {
            var = new std::string;
            type = TSTRING;
        }
        if( TSTRING == type )
        {
            *convertStringP( var ) = x;
            bSuccess = true;
        }
        return bSuccess;
    }
    bool Var::set( bool x )
    {
        bool bSuccess = false;
        if( TNOTDEFINE == type )
        {
            var = new bool;
            type = TBOOL;
        }
        if( TBOOL == type )
        {
            *convertBoolP( var ) = x;
            bSuccess = true;
        }

        return bSuccess;
    }
    bool Var::set( wizObject& x )
    {
        bool bSuccess = false;
        if( TOBJECT == type || TNOTDEFINE == type )
        {
            type = TOBJECT;
            if( NULL == var )
            {
                var = x.clone();
            }
            else
            {
                *convertObjectP( var ) = x;
            }
            bSuccess = true;
        }

        return bSuccess;
    }

    // toString -> pair<bool, std::string> ?
    std::string Var::toString()const
    {
        switch( type )
        {
        case TINT:
            return _toString( *convertIntP( var ) );
            break;
        case TDOUBLE:
            return _toString( *convertDoubleP( var ) );
            break;
        case TSTRING:
            return _toString( *convertStringP( var ) );
            break;
        case TBOOL:
            return _toString( *convertBoolP( var ) );
            break;
        case TOBJECT:
            return convertObjectP( var )->toString();
        case TNOTDEFINE:
            return "not defined";
        default:
            throw wizVarError();
        }
    }
}

#endif // WIZVAR_H_INCLUDED
