#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <string>
#include <math.h>
#include <eosiolib/system.hpp>
#include <eosiolib/crypto.hpp>
#include <eosiolib/multi_index.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/permission.h>
#include <eosiolib/time.hpp>

namespace eosiosystem {
   class system_contract;
}

namespace eosio {

   using std::string;

   class [[eosio::contract("sovmintofeos")]] token : public contract {
      public:
         using contract::contract;
         
         [[eosio::action]] 
         void selfburn( name account, asset quantity, string memo );
         
         
         [[eosio::action]] 
         void userretire( name sender, name reciever, asset burnamount, asset remaining,  string memo );
        

         [[eosio::action]] 
         void destroytoken(asset token);
         
          [[eosio::action]] 
          void destroyacc(asset token, name account);
         
         [[eosio::action]]
         void close( name owner, asset value );
         
         [[eosio::action]]
         void create( name   issuer,
                      asset  maximum_supply);

         [[eosio::action]]
         void issue( name to, asset quantity, string memo );

         [[eosio::action]]
         void retire( asset quantity, string memo );

         [[eosio::action]]
         void transfer( name    from,
                        name    to,
                        asset   quantity,
                        string  memo );
          
        
         
         [[eosio::action]] 
         void airgrab2( name owner, asset value);
        
         [[eosio::action]]
         void airburn(name account, asset amount);
         
         [[eosio::action]]
         void store(name account, asset value);
         
         [[eosio::action]]
         void withdraw(name account, asset value);

         static asset get_supply( name token_contract_account, symbol_code sym_code )
         {
            stats statstable( token_contract_account, sym_code.raw() );
            const auto& st = statstable.get( sym_code.raw() );
            return st.supply;
         }

         static asset get_balance( name token_contract_account, name owner, symbol_code sym_code )
         {
            accounts accountstable( token_contract_account, owner.value );
            const auto& ac = accountstable.get( sym_code.raw() );
            return ac.balance;
         }

         using create_action = eosio::action_wrapper<"create"_n, &token::create>;
         using issue_action = eosio::action_wrapper<"issue"_n, &token::issue>;
         using retire_action = eosio::action_wrapper<"retire"_n, &token::retire>;
         using transfer_action = eosio::action_wrapper<"transfer"_n, &token::transfer>;
         using close_action = eosio::action_wrapper<"close"_n, &token::close>;
      private:
         struct [[eosio::table]] account {
            asset    balance;
            asset    storebalance;
            asset    unstaking;
            uint32_t  unstake_time;

            uint64_t primary_key()const { return balance.symbol.code().raw(); }
         };

         struct [[eosio::table]] currency_stats {
            asset    supply;
            asset    max_supply;
            name     issuer;
            uint32_t burn_time;

            uint64_t primary_key()const { return supply.symbol.code().raw(); }
         };
         
         struct [[eosio::table]] signup {
           name account;
           
           
           
           auto primary_key() const {return account.value;}
           
         };

         typedef eosio::multi_index< "accounts"_n, account > accounts;
         typedef eosio::multi_index< "stat"_n, currency_stats > stats;
         typedef eosio::multi_index< "signuplist"_n, signup > signups;

         void sub_balance( name owner, asset value );
         void add_balance( name owner, asset value, name ram_payer );
         
   };

} /// namespace eosio
