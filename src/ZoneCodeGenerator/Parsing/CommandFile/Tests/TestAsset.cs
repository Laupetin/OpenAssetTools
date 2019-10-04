using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestAsset : AbstractTokenTest<ICommandParserState>
    {
        private const string AssetTypeNameToken = "name";
        private const string AssetEnumEntryToken = "enumEntry";

        private static readonly TokenMatcher[] matchers = {
            new MatcherLiteral("asset"),
            new MatcherTypename().WithName(AssetTypeNameToken),
            new MatcherName().WithName(AssetEnumEntryToken), 
            new MatcherLiteral(";")
        };

        public TestAsset() : base(matchers)
        {

        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            var assetTypeName = NextMatch(AssetTypeNameToken);

            var assetType = state.Repository.GetDataTypeByName(assetTypeName);
            if (assetType == null)
            {
                throw new LoadingException($"Could not find type '{assetTypeName}' to mark it as an asset.");
            }
            
            if (!(assetType is DataTypeWithMembers assetTypeWithMembers))
            {
                throw new LoadingException($"Type of asset '{assetTypeName}' needs to be struct or union.");
            }

            var assetInfo = state.Repository.GetInformationFor(assetTypeWithMembers);
            if (assetType == null)
            {
                throw new LoadingException($"Could not find information for type '{assetTypeName}' to mark it as an asset.");
            }

            var enumEntryName = NextMatch(AssetEnumEntryToken);
            var enumEntry = state.Repository.GetAllEnums()
                .SelectMany(_enum => _enum.Members)
                .FirstOrDefault(member => member.Name.Equals(enumEntryName, StringComparison.CurrentCultureIgnoreCase));

            assetInfo.AssetEnumEntry = enumEntry ?? throw new LoadingException(
                    $"Could not find enum entry '{enumEntryName}' as an asset type index for asset type '{assetTypeName}'.");
        }
    }
}
